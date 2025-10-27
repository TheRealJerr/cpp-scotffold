#include "../include/AMQP.h"
#include "../include/log.h"
#include <string_view>
#include <chrono>
using namespace std::chrono_literals;

namespace Tools
{


AMQP::ExchangeType transtype(Tools::ExchangeType type)
{
    if(type == ExchangeType::DIRECT) 
        return AMQP::ExchangeType::direct;
    else if(type == ExchangeType::FANOUT)
        return AMQP::ExchangeType::fanout;
    else if(type == ExchangeType::TOPIC)
        return AMQP::ExchangeType::topic;
    else if(type == ExchangeType::HEADERS)
        return AMQP::ExchangeType::headers;
    else if(type == ExchangeType::DELAYED)
        return AMQP::ExchangeType::direct;
    else {
        ERR("交换机类型异常{}", static_cast<int>(type));
        abort();
    }
}

AsyncAMQPClient::AsyncAMQPClient(std::string_view url):
    ev_loop_(EV_DEFAULT),
    handler_(ev_loop_),
    connection_(&handler_, std::string(url)),
    channel_(&connection_),
    is_declare_done_(false)
{}



// 发送消息
bool AsyncAMQPClient::publish(const std::string& ex_name,
    const std::string& rounte_key,
    const std::string& msg)
{
    
    auto ret = channel_.publish(ex_name, rounte_key, msg);
    if(ret)
        INF("发送数据成功");
    else 
        ERR("发送数据失败");
    return ret;
}

bool AsyncAMQPClient::consume(const std::string& que_name, 
    on_message_cb&& cb)
{
    channel_.consume(que_name)
        .onMessage([&, cb = std::move(cb)](const AMQP::Message& message, uint64_t deliveryTag, bool redelivered) {

            
            std::string body(message.body(), message.bodySize());
            // INF("收到消息:{}", body);
            if(cb) cb(body.c_str(), body.size());
            // 消息确认
            channel_.ack(deliveryTag);

        })
        .onSuccess([&](const std::string& consumer){
            INF("订阅消息成功 {}", consumer);
        })
        .onError([&](const char* message) {
            ERR("订阅消息失败 {}", message);
            abort();
        });
    return true;
}
// 等待队列声明和绑定完成
// void AsyncAMQPClient::wait_for_declare()
// {
//     if(is_declare_done_.load() == false)
//     {
//         std::unique_lock<std::mutex> lock(mtx_);
//         cond_.wait(lock);
//     }
// }

void AsyncAMQPClient::wait_for_working()
{
    if(work_thread_.joinable())
        work_thread_.join();
}


void AsyncAMQPClient::start()
{
    auto self = shared_from_this();
    work_thread_ = std::thread([self, this]()
        {
            ev_run(ev_loop_); // 启动事件循环
        });
}

void AsyncAMQPClient::stop()
{
    // 停止时间循环
    // 1. 初始化监视器
    ev_async_init(&ev_async_, stop_ev_callback);
    ev_async_start(ev_loop_, &ev_async_);    
    ev_async_send(ev_loop_, &ev_async_);
    if(work_thread_.joinable())
        work_thread_.join();
    INF("工作线程退出成功");
}

// // 声明普通队列
// void AsyncAMQPClient::_declare(const DeclareConfig& config, 
//         bool is_dead_queue,
//         const AMQP::Table& args)
// {
//     auto [ex_name, ex_type, que_name, bind_key, ttl] = config;
//     if(is_dead_queue)
//     {
//         ex_name = config.dlx_exchange();
//         que_name = config.dlx_queue();
//         ex_type = ExchangeType::DIRECT;
//         bind_key = config.dlx_bind_key();
//         ttl = 0;
//     }
//     channel_.declareExchange(ex_name, transtype(ex_type))
//        .onSuccess([=]() {
//             INF("声明交换区成功");

//             // 声明队列
//             auto on_success_cb = [&](const std::string &name, uint32_t messagecount, uint32_t consumercount)
//                 {
//                     INF("{}队列声明成功", name);
//                     INF("消息数量:{}", messagecount);
//                     INF("消费者数量:{}", consumercount);
//                 };
//             auto on_error_cb = [&](const char* message) 
//                 {
//                     INF("声明交换区失败:{}", message);
//                     abort();
//                 };
//             // 执行对应的回调函数
//             channel_.declareQueue(que_name, args)
//                .onSuccess(std::move(on_success_cb))
//                .onError(std::move(on_error_cb));
//             // 绑定交换机和队列
//             channel_.bindQueue(ex_name, que_name, bind_key)
//                .onSuccess([this]() {
//                     INF("绑定交换机和队列成功");
//                     // publish在外部实现
//                     std::unique_lock<std::mutex> lock(mtx_);
//                     cond_.notify_all();
//                })
//                .onError([&](const char* message) {
//                     ERR("绑定交换机和队列失败{}", message);
//                     abort();
//                });
            
//         })
//        .onError([&](const char* message) {
//             ERR("声明交换区失败{}", message);
//             abort();
//         });
//     wait_for_declare();
// }
// 声明普通队列
void AsyncAMQPClient::_declare(const DeclareConfig& config, 
        bool is_dead_queue,
        const AMQP::Table& args)
{
    auto [ex_name, ex_type, que_name, bind_key, ttl] = config;
    if(is_dead_queue)
    {
        ex_name = config.dlx_exchange();
        que_name = config.dlx_queue();
        ex_type = ExchangeType::DIRECT;
        bind_key = config.dlx_bind_key();
        ttl = config.ttl;
    }
    
    // 重置声明状态
    {
        std::unique_lock<std::mutex> lock(mtx_);
        is_declare_done_ = false;
    }
    
    auto self = shared_from_this();
    
    channel_.declareExchange(ex_name, transtype(ex_type))
       .onSuccess([self, que_name, ex_name, bind_key, args, this]() {
            INF("声明交换区成功");

            // 声明队列
            self->channel_.declareQueue(que_name, args)
               .onSuccess([self, que_name, ex_name, bind_key, this](const std::string &name, 
                                                                   uint32_t messagecount, 
                                                                   uint32_t consumercount) {
                    INF("{}队列声明成功", name);

                    // 绑定交换机和队列
                    self->channel_.bindQueue(ex_name, que_name, bind_key)
                       .onSuccess([self, this]() {
                            INF("绑定交换机和队列成功");
                            std::unique_lock<std::mutex> lock(mtx_);
                            is_declare_done_ = true;
                            cond_.notify_all();
                       })
                       .onError([self, this](const char* message) {
                            ERR("绑定交换机和队列失败: {}", message);
                            // 出错时也要通知，避免死锁
                            std::unique_lock<std::mutex> lock(mtx_);
                            is_declare_done_ = true;
                            cond_.notify_all();
                       });
               })
               .onError([self, this](const char* message) {
                    ERR("声明队列失败: {}", message);
                    std::unique_lock<std::mutex> lock(mtx_);
                    is_declare_done_ = true;
                    cond_.notify_all();
               });
        })
       .onError([self, this](const char* message) {
            ERR("声明交换区失败: {}", message);
            std::unique_lock<std::mutex> lock(mtx_);
            is_declare_done_ = true;
            cond_.notify_all();
        });
    
    // 等待声明完成
    wait_for_declare();

    // 由于死信队列可能要declare两次, 所以这里会将declare_done_重置为false
    is_declare_done_.store(false);
}

// 修改 wait_for_declare 方法
void AsyncAMQPClient::wait_for_declare()
{
    std::unique_lock<std::mutex> lock(mtx_);
    // 添加条件检查和超时
    if(is_declare_done_.load() == false)
    {
        cond_.wait_for(lock, 10s, [this]() { 
            return is_declare_done_.load(); 
        });
        
        if (!is_declare_done_) {
            ERR("等待队列声明超时");
        }
    }else return;
}

// 声明队列
void AsyncAMQPClient::declare(const DeclareConfig& config)
{
    bool is_dead_queue = false;
    if(config.ex_type == ExchangeType::DELAYED)
        is_dead_queue = true;
    if(is_dead_queue == false){
        _declare(config, false);
    }else
    {
        // 声明死信队列
        _declare(config, true);
        AMQP::Table args;
        args["x-dead-letter-exchange"] = config.dlx_exchange();
        args["x-dead-letter-routing-key"] = config.dlx_bind_key();

        
        if(config.ttl <= 0)
        {
            ERR("延迟队列TTL必须大于0");
            abort();
        }
        args["x-message-ttl"] = config.ttl;
        // 声明普通队列并关联死信队列
        _declare(config, false, args);
    }
}



void AsyncAMQPClient::stop_ev_callback(struct ev_loop* loop, ev_async* watcher, int32_t revents)
{
    ev_break(loop, EVBREAK_ALL);
}

AMQPPublish::AMQPPublish(client_ptr_t client,const DeclareConfig& config) : 
    client_(client), 
    config_(config)
{}

void AMQPPublish::publish(const std::string& msg)
{
    client_->declare(config_);
    client_->publish(config_.ex_name,config_.bind_key, msg);

}

AMQPSubscribe::AMQPSubscribe(client_ptr_t client,const DeclareConfig& config): 
    client_(client),
    config_(config)
{}

void AMQPSubscribe::consume(on_message_cb&& cb)
{
    client_->declare(config_);
    if(config_.ex_type == ExchangeType::DELAYED)
    {
        client_->consume(config_.dlx_queue(), std::move(cb));
    }else 
    {
        client_->consume(config_.que_name, std::move(cb));
    }
}

}