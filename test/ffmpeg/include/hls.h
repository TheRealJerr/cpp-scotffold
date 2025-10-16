#pragma once
// 这个文件的功能
// 1. 进行视频的转码操作
// 2. 封装hls信息
extern "C" {
    #include <libavformat/avformat.h>   
    #include <libavcodec/avcodec.h>   
    #include <libavutil/opt.h>
    #include <libavutil/error.h>
    #include <libavutil/time.h>
}
#include <iostream>
#include <vector>
#include <string>
#include "util.h"
#include "log.h"



namespace Tools
{
    // HLS文件格式的配置字段
    const std::string HLS_EXTM3U = "#EXTM3U";
    const std::string HLS_VERSION = "#EXT-X-VERSION:";
    const std::string HLS_TARGETDURATION = "#EXT-X-TARGETDURATION:";
    const std::string HLS_SEQUENCE = "EXT-X-MEDIA-SEQUENCE:";
    const std::string HLS_PLAYLIST_TYPE = "EXT-X-PLAYLIST-TYPE";
    const std::string HLS_INDEPENDENT_SEGMENTS = "#EXT-X-INDEPENDENT-SEGMENTS";
    const std::string HLS_ENDLIST = "#EXT-X-ENDLIST";
    const std::string HLS_EXTINF = "#EXTINF:";


    class M3U8Info
    {
        using pair_t = std::pair<std::string, std::string>;
        std::string filename_;
        std::vector<std::string> headers_;
        std::vector<pair_t> pieces_; // 切片信息
        
    public:

        explicit M3U8Info(const std::string& filename) : filename_(filename) {}

        explicit M3U8Info() = default;

        // 获取文件名
        void set_filename(const std::string& filename) { filename_ = filename; }

        const std::string& get_filename() const { return filename_; }

        // 解析m3u8文件
        bool parse();

        // 写入m3u8文件
        bool write();

        // 获取头部字段
        std::vector<std::string>* get_mutiple_headers();

        const std::vector<std::string>& get_headers() const;

        // 获取切片信息
        std::vector<pair_t>* get_mutiple_pieces();

        const std::vector<pair_t>& get_pieces() const;
    };
    struct HLSTranscoderConfig
    {
        size_t hls_time;
        std::string base_url;
        std::string play_list_type;
    };
    // HLS转码器
    class HLSTranscoder
    {
        HLSTranscoderConfig config_;
    public:

        // @param: HLSTranscoderConfig -- 配置信息
        HLSTranscoder(const HLSTranscoderConfig& config) : config_(config) {}
        // 转码
        bool transcode(const std::string& inputFile, const std::string& outputFile);
        // 错误信息
        const char* ErrorMessage(int error_code);
    };
}