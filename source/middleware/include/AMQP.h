#pragma once
#include <amqpcpp.h>
#include <ev.h>
#include <amqpcpp/libev.h>
#include <string>
#include <string_view>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <atomic>
#include <chrono>

namespace Tools
{

class AsyncAMQPClient;

// 交换机类型
enum class ExchangeType
{
    DIRECT,
    FANOUT,
    TOPIC,
    HEADERS,
    DELAYED,
};
// 套件的配置结构体
struct DeclareConfig
{
    std::string ex_name;
    ExchangeType ex_type;
    std::string que_name;
    std::string bind_key;
    size_t ttl = 0;

    inline std::string dlx_exchange() const { return "dlx_" + ex_name; }
    inline std::string dlx_queue() const { return "dlx_" + que_name; }
    inline std::string dlx_bind_key() const { return "dlx_" + bind_key; }
};

// 类型的转换
AMQP::ExchangeType transtype(Tools::ExchangeType);

// 异步客户端
using client_ptr_t = std::shared_ptr<AsyncAMQPClient>;
using on_message_cb = std::function<void(const char*, size_t)>;

class AsyncAMQPClient : public std::enable_shared_from_this<AsyncAMQPClient>
{
    struct ev_loop * ev_loop_;;
    struct ev_async ev_async_;
    AMQP::LibEvHandler handler_;
    AMQP::TcpConnection connection_;
    AMQP::TcpChannel channel_;

    std::mutex mtx_;
    std::condition_variable cond_;
    std::thread work_thread_; // 工作线程(访问RabbitMQ)
    std::atomic_bool is_declare_done_; // 声明队列是否完成
    
public:
    AsyncAMQPClient(std::string_view url);

    // 声明队列
    void declare(const DeclareConfig& config);

    // 发送消息
    bool publish(const std::string& ex_name,
        const std::string& rounte_key,
        const std::string& msg);

    bool consume(const std::string& que_name, 
        on_message_cb&& cb);
    // 等待declare结束
    
    // 等待工作线程
    void wait_for_working();

    void start();

    void stop();

    ~AsyncAMQPClient() { stop(); }

private:
    void wait_for_declare();

    // 声明队列和交换机
    // 1.普通队列bind
    void _declare(const DeclareConfig& config, 
        bool is_dead_queue = false,
        const AMQP::Table& args = AMQP::Table());
    
    // 发送终止的任务
    static void stop_ev_callback(struct ev_loop* loop, 
        ev_async* watcher, 
        int32_t revents);
};

// 消息的发布者
class Publish
{
public:

    virtual void publish(const std::string& msg) = 0;

    virtual ~Publish() = default;
};

// 消息的订阅者
class Subscribe
{
public:
    virtual void consume(on_message_cb&& cb) = 0;

    virtual ~Subscribe() = default;
};

class AMQPPublish : public Publish
{
    client_ptr_t client_;
    DeclareConfig config_;

public:
    AMQPPublish(client_ptr_t client,const DeclareConfig& config);

    virtual void publish(const std::string& msg) override;
};

class AMQPSubscribe : public Subscribe
{
    client_ptr_t client_;
    DeclareConfig config_;
    on_message_cb cb_;
public:
    AMQPSubscribe(client_ptr_t client,const DeclareConfig& config);

    virtual void consume(on_message_cb&& cb) override;
};



}