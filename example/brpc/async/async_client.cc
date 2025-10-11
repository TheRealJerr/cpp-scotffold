// 异步Rpc的客户端
#include <brpc/channel.h>
#include <thread>
#include "caculate.pb.h"

using namespace caculate;

void on_message(brpc::Controller* cntl, AddResponse* rsp)
{
    if(cntl->Failed())
    {
        std::cout << "处理失败:" << cntl->ErrorText() << std::endl;
        return;
    }
    std::cout << "result: " << rsp->result() << std::endl;
}

struct Wrapper
{
    Wrapper(std::function<void()> callback) : callback_(std::move(callback)) {}
    std::function<void()> callback_;

    static void static_callback(Wrapper* wrapper)
    {
        std::unique_ptr<Wrapper> wrapper_guard(wrapper);
        wrapper->callback_();
    }
};
// Closure的创建使用NewCallback函数, 但是这个函数的参数只支持函数指针, 因此我们针对NewCallback进行再封装, 封装了工厂类, 使得支持lambda表达式和仿函数
struct CreateClosureFactory
{
    using callback_t = std::function<void(void)>;

    template <class Func, class... Args>
    static google::protobuf::Closure* create_closure(Func&& func, Args&&... args)
    {
        auto callback = [func = std::forward<Func>(func), 
                        args_tuple = std::make_tuple(std::forward<Args>(args)...)]() mutable {
            std::apply([&](auto&&... captured_args) {
                func(captured_args...);
            }, args_tuple);
        };
        
        Wrapper* wrapper = new Wrapper(std::move(callback));
        return google::protobuf::NewCallback(Wrapper::static_callback, wrapper);
    }
};

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
    google::protobuf::Closure* done = CreateClosureFactory::create_closure(test_on_msg_hdl, &cntl, &rsp);
    
    stub.Add(&cntl, &req, &rsp, done);
    
    
    std::cout << "等待结果..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    return 0;
}