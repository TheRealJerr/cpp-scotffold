#include <brpc/server.h>
#include <brpc/log.h>
#include <butil/logging.h>
#include "caculate.pb.h"
#include <thread>

using namespace caculate;

// 关闭日志服务
class CaculateServiceImpl : public CaculateService {

public:

    virtual void Add(google::protobuf::RpcController* controller,
                    const AddRequest* request,
                    AddResponse* response,
                    google::protobuf::Closure* done) override
    {
        std::thread([=](){
            brpc::ClosureGuard done_guard(done);

            int result = request->num1() + request->num2();

            response->set_result(result);

            std::cout << "caculate finished" << std::endl;
        }).detach();

    }
};


int main()
{
    /*
        1. 实例化计算服务对象
        2. 实例化Server对象
        3. 注册对应的服务
    */
    CaculateServiceImpl service;
    
    brpc::ServerOptions options;

    options.idle_timeout_sec = -1;
    
    brpc::Server server;
    
    int ret = server.AddService(&service, brpc::SERVER_DOESNT_OWN_SERVICE);

    if(ret == -1)
    {
        std::cout << "Add Service Failed" << std::endl;
        return -1;
    }

    ret = server.Start(9000, &options);

    if(ret == -1)
    {
        std::cout << "Start Server Failed" << std::endl;
        return -1;
    }
    
    server.RunUntilAskedToQuit();

    return 0;
}


