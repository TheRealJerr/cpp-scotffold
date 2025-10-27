// Add服务的提供者

#include <brpc/server.h>
#include <brpc/log.h>
#include <butil/logging.h>
#include "../test.pb.h"
#include "../include/log.h"
#include "../include/etcd.h"

const std::string etcd_addr = "http://127.0.0.1:2379";

class GreetingServiceImpl : public test::Greeting {
public:
    virtual void Hello(google::protobuf::RpcController* controller,
             const test::HelloRequest* request,
             test::HelloResponse* response,
             google::protobuf::Closure* done) override
    {
        brpc::ClosureGuard done_guard(done);
        response->set_message("get hello from provider1");
        INF("Hello from provider1");
    }
};


int main()
{
    /*
        1. 实例化计算服务对象
        2. 实例化Server对象
        3. 注册对应的服务
    */
    init_global_logging();

    GreetingServiceImpl service;
    
    brpc::ServerOptions options;

    options.idle_timeout_sec = -1;
    
    brpc::Server server;

    int ret = server.AddService(&service, brpc::SERVER_DOESNT_OWN_SERVICE);
    if(ret == -1)
    {
        std::cout << "Add Service Failed" << std::endl;
        return -1;
    }

    ret = server.Start(9001, &options);

    if(ret == -1)
    {
        std::cout << "Start Server Failed" << std::endl;
        return -1;
    }
    // 进行服务的注册
    std::shared_ptr<Tools::ServiceProvider> provider
        = std::make_shared<Tools::ServiceProvider>(etcd_addr);
    // 注册服务
    provider->register_service("127.0.0.1:9001", "Greeting");
    
    server.RunUntilAskedToQuit();
    // 服务下线通知
    provider->deregister_service();
    return 0;
}