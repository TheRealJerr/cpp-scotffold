#include <iostream>
#include <hls.h>
#include <command.h>
using namespace Tools;
void test_create_m3u8(int argc, char* argv[])
{
    if(argc != 3)
    {
        std::cout << "Usage input "  << std::endl;
        return;
    }

    std::string input_file = argv[1];
    std::string output_file = argv[2];
    HLSTranscoderConfig config = {
        .hls_time = 10,
        .base_url = "http://192.168.124.148:9000/vedio/",
        .play_list_type = "vod",
    };

    HLSTranscoder transcoder(config);

    if(!transcoder.transcode(input_file, output_file))
        std::cout << "transcode failed" << std::endl;
    else 
        std::cout << "transcode success" << std::endl;
}

void test_parse_M3U8(const std::string& filename)
{
    M3U8Info info(filename);
    bool status = info.parse();
    if(!status)
    {
        std::cout << "parse failed" << std::endl;
        return;
    }
    // 
    // do something with info

    auto& header = info.get_headers();
    for(auto& h : header)
        std::cout << "[" << h << "]" << std::endl;
    auto& pieces = info.get_pieces();
    for(auto& p : pieces)
        std::cout << "key:" << p.first << " value:" << p.second << std::endl;
    // 写入信息
    auto* mutiple_headers = info.get_mutiple_headers();
    mutiple_headers->push_back("test");
    mutiple_headers->push_back("test2");
    
    auto* mutiple_pieces = info.get_mutiple_pieces();
    mutiple_pieces->push_back(std::make_pair("key1", "value1"));
    mutiple_pieces->push_back(std::make_pair("key2", "value2"));

    info.write();
}
int main(int argc, char* argv[])
{
    init_global_logging();
    const std::string filename = argv[1];
    test_parse_M3U8(filename);
    return 0;
}