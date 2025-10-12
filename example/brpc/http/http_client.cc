#include <iostream>
#include <brpc/channel.h>
#include "../caculate.pb.h"

using namespace caculate;


int main(){
    // 1. 实例化一个Channel
    brpc::ChannelOptions options;
    options.protocol = brpc::PROTOCOL_HTTP;

    brpc::Channel channel;
    channel.Init("192.168.124.148:9000", &options);   

    brpc::Controller cntl;
    auto& http_request = cntl.http_request();
    http_request.set_method(brpc::HTTP_METHOD_POST);
    // 设置请求的urln
    http_request.uri().set_path("/CaculateService/Hello");
    
    cntl.request_attachment().append("hello world");

    channel.CallMethod(nullptr, &cntl, nullptr, nullptr, nullptr);
    // 获取相应
    if(cntl.Failed())
    {
        std::cout << "Failed to call method" << cntl.ErrorText() << std::endl;
        return -1;
    }else 
    {
        std::string response_str = cntl.response_attachment().to_string();
        std::cout << "response_str: " << response_str << std::endl;
    }

    return 0;
}