
/*
    1. 实例化libev网络互的通信句柄
    2. 实例化Connection对象
    3. 实例化Channel对象
    4. 声明交换机
    5. 绑定交换机和队列
    6. 声明消息
*/

#include <amqpcpp.h>
#include <ev.h>
#include <amqpcpp/libev.h>
#include <iostream>

// 定义rabbitmq的URL
const std::string rabbitmq_url = "amqp://admin:123456@192.168.124.148:5672/";

int main()
{
    const std::string exchange_name = "myexchange";
    const std::string queue_name = "myqueue";
    const std::string binding_key = "my-key";
    const std::string routing_key = "my-key";
    // 1. 
    auto* event_loop = EV_DEFAULT;
    AMQP::LibEvHandler handler(event_loop);

    AMQP::TcpConnection connection(&handler, AMQP::Address(rabbitmq_url));

    AMQP::TcpChannel channel(&connection);


    // 直接交换
    channel.declareExchange(exchange_name, AMQP::ExchangeType::direct)
       .onSuccess([&]() {
            std::cout << "声明交换区成功" << std::endl;

            // 声明队列
            auto on_success_cb = [&](const std::string &name, uint32_t messagecount, uint32_t consumercount)
                {
                    std::cout << name << "队列声明成功" << std::endl;
                    std::cout << "消息数量：" << messagecount << std::endl;
                    std::cout << "消费者数量：" << consumercount << std::endl;
                };
            auto on_error_cb = [&](const char* message) 
                {
                    std::cerr << "声明交换区失败" << message << std::endl;
                    abort();
                };
            // 执行对应的回调函数
            channel.declareQueue(queue_name)
               .onSuccess(std::move(on_success_cb))
               .onError(std::move(on_error_cb));
            // 绑定交换机和队列
            channel.bindQueue(exchange_name, queue_name, binding_key)
               .onSuccess([&]() {
                    std::cout << "绑定交换机和队列成功" << std::endl;

                    bool ret = channel.publish(exchange_name, routing_key, "Hello World!");

                    if(ret)
                    {
                        std::cout << "消息发送成功" << std::endl;
                    }else {
                        std::cout << "消息发送失败" << std::endl;
                    }
               })
               .onError([&](const char* message) {
                    std::cerr << "绑定交换机和队列失败" << message << std::endl;
                    abort();
               });
            
        })
       .onError([&](const char* message) {
            std::cerr << "声明交换区失败" << message << std::endl;
            abort();
        });
    // 启动事件循环
    ev_run(event_loop);
    
    ev_break(event_loop);

    return 0;
}