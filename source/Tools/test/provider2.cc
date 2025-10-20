// Add服务的提供者

#include <brpc/server.h>
#include <brpc/log.h>
#include <butil/logging.h>
#include "../test.pb.h"
#include "../include/log.h"
#include "../include/etcd.h"

const std::string etcd_addr = "http://127.0.0.1:2379";


class AddServiceImpl : public test::Math {
public:
    virtual void Add(google::protobuf::RpcController* controller,
             const test::AddRequest* request,
             test::AddResponse* response,
             google::protobuf::Closure* done) override
    {
        brpc::ClosureGuard guard(done);
        int result = request->num1() + request->num2();
        INF("AddServiceImpl::Add result={}", result);
        response->set_result(result);
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
    AddServiceImpl service;
    
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
    std::shared_ptr<Tools::ServiceProvider> provider
        = std::make_shared<Tools::ServiceProvider>(etcd_addr);
    // 注册服务
    provider->register_service("127.0.0.1:9000", "Add");

    server.RunUntilAskedToQuit();

    provider->deregister_service();

    return 0;
}