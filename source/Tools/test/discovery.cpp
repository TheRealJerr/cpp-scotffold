// 发现服务的客户端 ../include/
#include "../include/command.hpp"
#include "../include/log.h"
#include "../include/etcd.h"
#include "../include/rigistry.h"
#include "../include/util.h"
#include "../test.pb.h"

const std::string etcd_addr = "http://127.0.0.1:2379";

int main()
{
    init_global_logging();

    /*
    1. 首先通过服务发现发现服务
    2. 通过连接池管理连接
    */
    
    // 管理连接池
    RpcTools::ChannelsPool pool;
    // 设置关注的服务
    pool.set_watch_services({"Add", "Greeting"});

    // 定义服务上线的回调函数

    auto online_cb = [&pool](const std::string& service_name, const std::string& endpoint)
        {
            INF("{}服务上线了, endpoint={}", service_name, endpoint);
            // 添加节点管理
            pool.add_node(service_name, endpoint);
        };
    auto offline_cb = [&pool](const std::string& service_name, const std::string& endpoint)
        {
            INF("{}服务下线了, endpoint={}", service_name, endpoint);
            // 
            pool.remove_node(service_name, endpoint);
        };

    // 创建一个服务的发现者
    auto discovery = std::make_shared<Tools::ServiceDiscovery>(etcd_addr, online_cb, offline_cb);

    // 进行服务的监测
    discovery->watch_service();
    getchar();

    // 尝试获取对应的拂去
    auto add_channel = pool.get_node("Add");
    if(!add_channel){
        ERR("获取Add服务失败");
    }else 
    {
        brpc::Controller* cntl = new brpc::Controller();
        auto on_msg_cb = [](brpc::Controller* cntl, test::AddResponse* rsp)
            {
                INF("收到Add服务的响应: {}", rsp->result());
            };
        // 构建Stub对象
        test::Math_Stub stub(add_channel.get());
        test::AddResponse* rsp = new test::AddResponse();
        google::protobuf::Closure* done = RpcTools::CreateClosureFactory::create_closure(on_msg_cb, cntl, rsp);
        // 调用请求
        test::AddRequest req;
        req.set_num1(10);
        req.set_num2(20);
        stub.Add(cntl, &req, rsp, done);
        INF("发送请求");
    }

    auto greeting_channel = pool.get_node("Greeting");
    if(greeting_channel.get() == nullptr){
        ERR("获取Greeting服务失败");
    }else 
    {
        brpc::Controller* cntl = new brpc::Controller();
        auto on_msg_cb = [](brpc::Controller* cntl, test::HelloResponse* rsp)
            {
                INF("收到Greeting服务的响应: {}", rsp->message());
            };
        // 构建Stub对象
        test::Greeting_Stub stub(greeting_channel.get());
        test::HelloResponse* rsp = new test::HelloResponse();
        google::protobuf::Closure* done = RpcTools::CreateClosureFactory::create_closure(on_msg_cb, cntl, rsp);
        // 调用请求
        test::HelloRequest req;
        stub.Hello(cntl, &req, rsp, done);
    }
    std::cout << "回车退出" << std::endl;
    getchar();
    return 0;
}