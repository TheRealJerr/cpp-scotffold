#include "rigistry.h"
#include "log.h"
#include "caculate.pb.h"
#include "command.h"
#include <brpc/channel.h>
using namespace RpcTools;

using namespace caculate;
void test_rpc_rigistry()
{
    // 1. 添加服务端节点管理
    // 2. 获取节点对应的channel
    // 3. 通过Stub对象发送
    // 初始化日志器
    init_global_logging();
    
    ChannelsPool channels_pool;
    
    channels_pool.set_watch_services({"usr"});
    channels_pool.add_node("usr", "192.168.124.148:9000");

    auto channel = channels_pool.get_node("usr");

    if(!channel){
        ERR("没有对应的服务");
        return;
    }
    auto* cntl = new brpc::Controller();

    AddRequest req;
    req.set_num1(10);
    req.set_num2(20);

    AddResponse rsp;
    auto test_on_msg_hdl = [](brpc::Controller* cntl, AddResponse* rsp)
        {
            if(cntl->Failed())
            {
                std::cout << "处理失败:" << cntl->ErrorText() << std::endl;
                return;
            }
            std::cout << "result: " << rsp->result() << std::endl;
        };

    // 注册回调函数
    google::protobuf::Closure* done = CreateClosureFactory::create_closure(test_on_msg_hdl, cntl, &rsp);
    brpc::ClosureGuard done_guard(done);
    // 创建Stub对象
    caculate::CaculateService_Stub stub(channel.get());
    
    stub.Add(cntl, &req, &rsp, done);

    // 等待回调函数执行
    std::cout << "=========================== " << std::endl;
    getchar();
}

int main()
{
    test_rpc_rigistry();
    return 0;
}