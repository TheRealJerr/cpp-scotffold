// epoll_dispatcher.h
#ifndef EPOLL_DISPATCHER_H
#define EPOLL_DISPATCHER_H

#include <memory>
#include <unordered_map>
#include <vector>
#include <sys/epoll.h>
#include "epoll_event.h"
#include "concrete_events.h"

class EpollDispatcher {

public:
    using event_t = std::shared_ptr<Event>;
    explicit EpollDispatcher(int max_events = 1024);
    ~EpollDispatcher();
    
    // 禁止拷贝
    EpollDispatcher(const EpollDispatcher&) = delete;
    EpollDispatcher& operator=(const EpollDispatcher&) = delete;
    
    // 允许移动
    EpollDispatcher(EpollDispatcher&& other) noexcept;
    EpollDispatcher& operator=(EpollDispatcher&& other) noexcept;
    
    // 添加/修改事件
    bool add_event(event_t event);
    bool modify_event(event_t event);
    bool delete_event(int fd);                         
    
    // 事件循环
    void run(int timeout_ms = -1);
    void stop() { running_ = false; }
    
    // 获取事件数量
    size_t get_event_count() const { return events_.size(); }
    
    // 检查是否包含某个文件描述符
    bool contains(int fd) const { return events_.find(fd) != events_.end(); }

private:
    int epoll_fd_;
    int max_events_;
    bool running_;
    std::vector<epoll_event> ready_events_;
    std::unordered_map<int, std::shared_ptr<Event>> events_;
    
    void cleanup();
};

#endif // EPOLL_DISPATCHER_H