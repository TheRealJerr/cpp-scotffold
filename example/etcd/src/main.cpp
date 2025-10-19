#include <iostream>
#include <fmt/format.h>
/*
    1. 创建客户端对象
    2. 添加数据
    3. 
*/


#include <iostream>
#include <etcd/Client.hpp>
#include <etcd/KeepAlive.hpp>
#include <etcd/Watcher.hpp>
#include <thread>

const std::string etcd_addr = "http://127.0.0.1:2379";

void test_etcd()
{
    // 创建一个客户端对象
    etcd::Client etcd("http://127.0.0.1:2379");
    
    auto rsp = etcd.put("hello", "world");
    // 
    if(rsp.wait())
    {
        auto msg = rsp.get();
        if(msg.is_ok())
        {
            std::cout << "put sucess" << std::endl;
            std::cout << "result: " << msg.value() << std::endl;
        }else
        {
            std::cout << "put failed" << std::endl;
            std::cout << "error: " << msg.error_message() << std::endl;
        }
    }
}

// get的测试请求
void test_etcd_get()
{
    etcd::Client etcd("http://127.0.0.1:2379");
    auto rsp = etcd.ls("/").get();
    if(rsp.is_ok())
    {
        std::cout << "get sucess" << std::endl;
        std::cout << "key : " << rsp.values().front().key() << std::endl;
        std::cout << "result: " << rsp.values().front().as_string() << std::endl;
    }else
    {
        std::cout << "get failed" << std::endl;
        std::cout << "error: " << rsp.error_message() << std::endl;
    }
}
// 测试租约的功能
void test_subscribe(etcd::Client& etcd)
{
    // 创建租约对象
    auto lease_rsp = etcd.leasegrant(3);
    if(lease_rsp.get().is_ok() == false)
    {
        std::cout << "创建租约失败" << std::endl;
        return;
    }
    // 获取租约的id
    auto lease_id = lease_rsp.get().leases().front();
    // 租约报货
    auto handler = [](std::exception_ptr e)
        {
        };
    etcd::KeepAlive keep_alive(etcd,handler, 3, lease_id);
    
    

}

// 测试watcher的功能
// 定义检测的回调函数
void on_watch_event(etcd::Response const& rsp)
{
    if(rsp.is_ok() == false)
    {
        std::cout << "watch failed" << std::endl;
        return;
    }
    auto const& events = rsp.events();
    for(auto const& event : events)
    {
        const auto& type = event.event_type();
        if(type == etcd::Event::EventType::PUT)
        {
            std::cout << "put event:\n" << 
                "prev value:" << event.prev_kv().as_string() << "\n" <<
                "current value:" << event.kv().as_string() << std::endl;
        }
        else if(type == etcd::Event::EventType::DELETE_)
        {
            std::cout << "delete event:\n" << 
                "prev value:" << event.prev_kv().as_string() << std::endl;
        }
        else 
        {
            std::cout << "invalid event type" << std::endl;
        }
    }
}
void test_etcd_watch(etcd::Client& etcd)
{
    etcd::Watcher watcher(etcd, "hello", on_watch_event);
    watcher.Wait();
}

void test_etcd_set(etcd::Client& etcd)
{
    auto rsp = etcd.set("hello", "bit");
    if(rsp.wait())
    {
        auto msg = rsp.get();
        if(msg.is_ok())
        {
            std::cout << "set sucess" << std::endl;
            std::cout << "result: " << msg.value() << std::endl;
        }else
        {
            std::cout << "set failed" << std::endl;
            std::cout << "error: " << msg.error_message() << std::endl;
        }
    }
}

void test_const_feature()
{

}

int main()
{
    etcd::Client etcd(etcd_addr);
    // 创建监控线程
    test_etcd_get();
    return 0;
}
