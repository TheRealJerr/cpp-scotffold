// reactor.h
#ifndef REACTOR_H
#define REACTOR_H

#include "../include/epoll_dispatcher.h"
#include <memory>
#include <thread>
#include <atomic>

class Reactor {
public:
    Reactor();
    ~Reactor();
    
    // 启动和停止 Reactor
    void start();
    void stop();
    
    // 注册事件
    bool registerEvent(std::shared_ptr<Event> event);
    bool unregisterEvent(int fd);
    bool updateEvent(std::shared_ptr<Event> event);
    
    // 获取底层 dispatcher
    EpollDispatcher& getDispatcher() { return dispatcher_; }
    
    // 检查是否运行中
    bool isRunning() const { return running_; }

private:
    EpollDispatcher dispatcher_;
    std::thread worker_thread_;
    std::atomic<bool> running_;
    
    void run();
};

#endif // REACTOR_H