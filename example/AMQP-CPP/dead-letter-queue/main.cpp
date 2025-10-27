#include <amqpcpp.h>
#include <ev.h>
#include <amqpcpp/libev.h>
#include <string>
#include <string_view>
std::string_view rabbitmq_url = "amqp://admin:123456@192.168.124.148:5672/";

/*
    1. 创建两个普通的队列和两个普通的交换机
    2. 关联关系, 常规队列的消息Timerouting_key_normalt后将数据交换到我们的死信队列
*/
using declare_exchange_sucess_hdl = std::function<void()>;
using declare_queue_sucess_hdl = std::function<void()>;
using exchange_bind_queue_sucess_hdl = std::function<void(void)>;

void declare_and_bind(AMQP::TcpChannel& channel,
    std::string_view que_name,
    std::string_view exchage_name,
    std::string_view bind_key,
    std::string_view rounte_key,
    declare_exchange_sucess_hdl&& desh = {},
    declare_queue_sucess_hdl&& dqsh = {},
    exchange_bind_queue_sucess_hdl&& dbqs = {})
{
    // 实例化普通的队列和交换价
    channel.declareExchange(exchage_name, AMQP::ExchangeType::direct)
       .onSuccess([&]() {
            std::cout << "声明普通交换区成功" << std::endl;
            if(desh) desh();
            // 声明队列
            auto on_success_cb = [&](const std::string &name, uint32_t messagecount, uint32_t consumercount)
                {
                    std::cout << name << "队列声明成功" << std::endl;
                    if(dqsh) dqsh();
                    std::cout << "消息数量：" << messagecount << std::endl;
                    std::cout << "消费者数量：" << consumercount << std::endl;
                };
            auto on_error_cb = [&](const char* message) 
                {
                    std::cerr << "声明交换区失败" << message << std::endl;
                    abort();
                };
            // 执行对应的回调函数
            channel.declareQueue(que_name)
               .onSuccess(std::move(on_success_cb))
               .onError(std::move(on_error_cb));
            // 绑定交换机和队列
            channel.bindQueue(exchage_name, que_name, bind_key)
               .onSuccess([&]() {
                    std::cout << "绑定交换机和队列成功" << std::endl;
                    if(dbqs) dbqs();
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
}

void test_dead_queue()
{
    
    std::string_view exchange_name_normal = "myexchange-normal";
    std::string_view queue_name_normal = "myqueue-normal";
    std::string_view exchange_name_dq = "myexchange-normal";
    std::string_view queue_name_dq = "myqueue-normal";
    
    std::string_view binding_key_normal = "my-key-normal";
    std::string_view routing_key_normal = "my-key-normal";
    std::string_view binding_key_dq = "my-key-dq";
    std::string_view routing_key_dq = "my-key-dq";
    // 1. 
    auto* event_loop = EV_DEFAULT;
    AMQP::LibEvHandler handler(event_loop);

    AMQP::TcpConnection connection(&handler, AMQP::Address(std::string(rabbitmq_url)));

    AMQP::TcpChannel channel(&connection);


    // 普通队列声明交换区成功回调
    auto decl_sucess_hdr = [&]()->void
        {
            AMQP::Table args;
            args["x-dead-letter-exchange"] = exchange_name_dq;
            args["x-dead-letter-routing-key"] = routing_key_dq;
            args["x-message-ttl"] = 5000; // 过期时间五秒
        };
    // 声明bind普通的队列
    declare_and_bind(channel,
        queue_name_normal, 
        exchange_name_normal, 
        binding_key_normal,
        routing_key_normal,
        decl_sucess_hdr);
    // 声明bind死信队列
    declare_and_bind(channel,
        queue_name_dq, 
        exchange_name_dq, 
        binding_key_dq,
        routing_key_dq);

    channel.publish(exchange_name_normal,
        routing_key_normal,
        "Hello World");
    ev_run(event_loop);
}

int main()
{
    test_dead_queue();
    return 0;
}
