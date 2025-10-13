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
    class M3U8Info
    {
        using pair_t = std::pair<std::string, std::string>;
        std::string filename_;
        std::vector<std::string> headers_;
        std::vector<pair_t> pieces_; // 切片信息
        
    public:

        explicit M3U8Info(const std::string& filename) : filename_(filename) {}

        explicit M3U8Info() = default;

        void set_filename(const std::string& filename) { filename_ = filename; }

        const std::string& get_filename() const { return filename_; }

        bool parse();

        bool write();

        std::vector<std::string>* get_mutiple_headers();

        const std::vector<std::string>& get_headers() const;

        std::vector<pair_t>* get_mutiple_pieces();

        const std::vector<pair_t>& get_pieces() const;
    };
    struct HLSTranscoderConfig
    {
        size_t hls_time;
        std::string base_usl;
        std::string play_list_type;
    };
    // HLS转码器
    class HLSTranscoder
    {
        HLSTranscoderConfig config_;
    public:
        HLSTranscoder(const HLSTranscoderConfig& config) : config_(config) {}
        // 转码
        bool transcode(const std::string& input_file, const std::string& output_file);
    };
}