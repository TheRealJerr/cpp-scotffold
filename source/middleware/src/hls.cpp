#include "../include/hls.h"

namespace Tools
{
    
    // 解析m3u8文件
    bool M3U8Info::parse()
    {
        std::string content;
        bool status = FileTools::read_from_file(filename_, &content);

        if(!status){
            ERR("{}:M3U8文件读取数据失败", filename_);
            return false;
        }
    
        std::vector<std::string> lines = 
            StringTools::split(content, "\n");
        
        for(int i = 0;i < lines.size();i++)
        {
            auto& line = lines[i];
            if(line.empty()) 
            {
                ERR("{}:M3U8文件格式错误 {}", filename_, strerror(errno));
                return false;
            }
            else if(line == HLS_ENDLIST) {
                break;
            }
            else if(StringTools::regex_match(line, HLS_EXTINF + ".*")) {
                pieces_.push_back(std::make_pair(lines[i], lines[i + 1]));
                i++;
            }
            else {
                headers_.push_back(line);
            }
        }
        INF("{}:M3U8文件解析成功", filename_);
        return true;
    }

    // 写入m3u8文件
    bool M3U8Info::write()
    {
        std::stringstream oss;
        // 写入头部信息
        for(auto& header : headers_)
            oss << header << std::endl;
        // 写入切片信息
        for(auto& piece : pieces_)
        {
            oss << piece.first << std::endl;
            oss << piece.second << std::endl;
        }
        // 写入尾部信息
        oss << HLS_ENDLIST << std::endl;

        if(!FileTools::write_to_file(filename_, oss.str()))
        {
            ERR("{}:M3U8文件写入数据失败", filename_);
            return false;
        }

        return true; // 写入成功
    }

    std::vector<std::string>* M3U8Info::get_mutiple_headers()
    {
        return &headers_;
    }

    const std::vector<std::string>& M3U8Info::get_headers() const
    {
        return headers_;
    }
    std::vector<M3U8Info::pair_t>* M3U8Info::get_mutiple_pieces()
    {
        return &pieces_;
    }

    const std::vector<M3U8Info::pair_t>& M3U8Info::get_pieces() const
    {
        return pieces_;
    }

    const char* HLSTranscoder::ErrorMessage(int error_code)
    {
        static char error_buffer[1024];
        av_strerror(error_code, error_buffer, sizeof(error_buffer));
        return error_buffer;
    }

    // 转码格式
    bool HLSTranscoder::transcode(const std::string& inputFile, 
        const std::string& outputFile)
    {
        
        const char* input_file = inputFile.c_str();
        const char* output_file = outputFile.c_str();
        
        AVFormatContext *input_context = nullptr, *output_context = nullptr;
        int ret = avformat_open_input(&input_context, input_file, nullptr, nullptr);

        if (ret < 0)
        {
            ERR("创建上下文对象失败: {}", ErrorMessage(ret));
            return false;
        }

        ret = avformat_find_stream_info(input_context, nullptr);
        if (ret < 0)
        {
            ERR("获取视频元信息失败: {}", ErrorMessage(ret));
            avformat_close_input(&input_context);
            return false;
        }

        // 创建输出格式化上下文对象
        ret = avformat_alloc_output_context2(&output_context, nullptr, "hls", output_file);
        if (ret < 0)
        {
            ERR("创建输出上下文对象失败: {}", ErrorMessage(ret));
            avformat_close_input(&input_context);
            return false;
        }

        // 复制流信息并正确设置时间基
        for(size_t i = 0; i < input_context->nb_streams; i++)
        {
            AVStream* input_stream = input_context->streams[i];
            AVStream* output_stream = avformat_new_stream(output_context, nullptr);
            
            if (!output_stream) {
                ERR("创建输出流失败: {}", ErrorMessage(ret));

                continue;
            }

            // 复制编解码参数
            ret = avcodec_parameters_copy(output_stream->codecpar, input_stream->codecpar);
            if (ret < 0) {

                ERR("复制编解码参数失败: {}", ErrorMessage(ret));
                continue;
            }

            output_stream->codecpar->codec_tag = 0;
            
            // 设置正确的时间基
            if (input_stream->time_base.den > 0 && input_stream->time_base.num > 0) {
                output_stream->time_base = input_stream->time_base;
            } else {
                // 设置合理的时间基
                if (input_stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                    output_stream->time_base = av_make_q(1, 90000); // 视频常用时间基
                } else if (input_stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                    output_stream->time_base = av_make_q(1, input_stream->codecpar->sample_rate);
                } else {
                    output_stream->time_base = input_stream->time_base;
                }
            }

            output_stream->avg_frame_rate = input_stream->avg_frame_rate;
            output_stream->r_frame_rate = input_stream->r_frame_rate;
            
            // 复制显示宽高比
            output_stream->sample_aspect_ratio = input_stream->sample_aspect_ratio;
        }

        // 设置hls参数
        AVDictionary* dict = nullptr;
        av_dict_set_int(&dict, "hls_time", config_.hls_time, 0);
        av_dict_set(&dict, "hls_base_url", config_.base_url.c_str(), 0);
        av_dict_set(&dict, "hls_playlist_type", config_.play_list_type.c_str(), 0);
        av_dict_set(&dict, "hls_flags", "independent_segments", 0);
        
        // 如果没有输出文件，需要创建
        if (!(output_context->oformat->flags & AVFMT_NOFILE)) {
            ret = avio_open(&output_context->pb, output_file, AVIO_FLAG_WRITE);
            if (ret < 0) {
                ERR("打开输出文件失败: {}", ErrorMessage(ret));
                avformat_close_input(&input_context);
                avformat_free_context(output_context);
                return false;
            }
        }

        // 写入头部
        ret = avformat_write_header(output_context, &dict);
        if (ret < 0)
        {
            ERR("设置hls参数失败: {}", ErrorMessage(ret));
            avformat_close_input(&input_context);
            avformat_free_context(output_context);
            return false;
        }

        // 处理数据包
        AVPacket packet;
        int64_t last_pts[input_context->nb_streams] = {0};
        
        while(av_read_frame(input_context, &packet) >= 0)
        {
            AVStream* input_stream = input_context->streams[packet.stream_index];
            AVStream* output_stream = output_context->streams[packet.stream_index];

            // 修正时间戳
            if (packet.pts == AV_NOPTS_VALUE) {
                if (last_pts[packet.stream_index] != AV_NOPTS_VALUE) {
                    packet.pts = last_pts[packet.stream_index] + 1;
                    packet.dts = packet.pts;
                } else {
                    packet.pts = 0;
                    packet.dts = 0;
                }
            }
            
            last_pts[packet.stream_index] = packet.pts;

            // 正确的时间戳转换：从输入流时间基转换到输出流时间基
            av_packet_rescale_ts(&packet, input_stream->time_base, output_stream->time_base);

            packet.pos = -1;

            // 写入帧
            ret = av_interleaved_write_frame(output_context, &packet);
            if (ret < 0) {
                ERR("写入帧失败: {}", ErrorMessage(ret));
                avformat_close_input(&input_context);
                avformat_free_context(output_context);
                return false;
            }
            
            av_packet_unref(&packet);
        }

        // 写入尾部
        ret = av_write_trailer(output_context);
        if (ret < 0)
        {
            ERR("写入尾部信息失败: {}", ErrorMessage(ret));
            return false;
        }

        // 清理资源
        av_dict_free(&dict);
        if (output_context && !(output_context->oformat->flags & AVFMT_NOFILE)) {
            avio_closep(&output_context->pb);
        }
        avformat_close_input(&input_context);
        avformat_free_context(output_context);

        return true;
    }

}