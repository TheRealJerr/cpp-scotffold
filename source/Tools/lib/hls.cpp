#include "../include/hls.h"

namespace Tools
{
    bool M3U8Info::parse()
    {}

    
    // 解析m3u8文件
    bool M3U8Info::parse()
    {}

    // 写入m3u8文件
    bool M3U8Info::write()
    {}

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


}