
#include <brpc/channel.h>
#include "caculate.pb.h"

using namespace caculate;


int main(){
    // 1. 实例化一个Channel
    brpc::ChannelOptions options;
    options.protocol = "baidu_std";

    brpc::Channel channel;
    channel.Init("192.168.124.148:9000", &options);

    CaculateService_Stub stub(&channel);    

    brpc::Controller cntl;

    AddRequest req;
    req.set_num1(10);
    req.set_num2(20);

    AddResponse rsp;

    stub.Add(&cntl, &req, &rsp, NULL);

    if(cntl.Failed())
    {
        std::cout << cntl.ErrorText() << std::endl;
        return -1;
    }
    std::cout << "result: " << rsp.result() << std::endl;   

    return 0;
}