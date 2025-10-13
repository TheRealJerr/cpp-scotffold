// epoll_dispatcher.cpp
#include "../include/epoll_dispatcher.h"
#include "../include/log.h"
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <system_error>

using namespace Tools;

EpollDispatcher::EpollDispatcher(int max_events) 
    : max_events_(max_events), running_(false) {
    
    epoll_fd_ = epoll_create1(0);
    if (epoll_fd_ == -1) {
        throw std::system_error(errno, std::system_category(), "epoll_create1 failed");
    }
    
    ready_events_.resize(max_events_);
    INF("EpollDispatcher created with max_events: {}", max_events_);
}

EpollDispatcher::~EpollDispatcher() {
    stop();
    cleanup();
}

EpollDispatcher::EpollDispatcher(EpollDispatcher&& other) noexcept
    : epoll_fd_(other.epoll_fd_),
      max_events_(other.max_events_),
      running_(other.running_),
      ready_events_(std::move(other.ready_events_)),
      events_(std::move(other.events_)) {
    other.epoll_fd_ = -1;
    other.running_ = false;
}

EpollDispatcher& EpollDispatcher::operator=(EpollDispatcher&& other) noexcept {
    if (this != &other) {
        cleanup();
        epoll_fd_ = other.epoll_fd_;
        max_events_ = other.max_events_;
        running_ = other.running_;
        ready_events_ = std::move(other.ready_events_);
        events_ = std::move(other.events_);
        
        other.epoll_fd_ = -1;
        other.running_ = false;
    }
    return *this;
}

bool EpollDispatcher::add_event(event_t event) {
    if (!event) {
        ERR("Cannot add null event");
        return false;
    }
    
    int fd = event->get_fd();
    epoll_event ev = event->transfrom_to_epoll_event();
    ev.data.ptr = event.get();  // 存储原始指针用于快速访问
    
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev) == -1) {
        ERR("Failed to add event for fd {} : {}" , fd, strerror(errno));
        return false;
    }
    
    events_[fd] = std::move(event);
    ERR("Event added for fd: {} : {}" , fd, strerror(errno));
    return true;
}

bool EpollDispatcher::modify_event(event_t event) {
    if (!event) {
        return false;
    }
    
    int fd = event->get_fd();
    if (events_.find(fd) == events_.end()) {
        ERR("Cannot modify non-existent event for fd {}", fd);
        return false;
    }
    
    epoll_event ev = event->transfrom_to_epoll_event();
    ev.data.ptr = event.get();
    
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev) == -1) {
        ERR("Failed to modify event for fd {} : {}" , fd, strerror(errno));
        return false;
    }
    
    events_[fd] = std::move(event);
    ERR("Event modified for fd: {} : {}" , fd, strerror(errno));    
    return true;
}

bool EpollDispatcher::delete_event(int fd) {
    auto it = events_.find(fd);
    if (it == events_.end()) {
        return true;  // 已经不存在，认为删除成功
    }
    
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr) == -1) {
        ERR("Failed to delete event for fd {} : {}" , fd, strerror(errno));
        return false;
    }
    
    events_.erase(it);
    INF("Event deleted for fd: {} : {}" , fd, strerror(errno));
    return true;
}

void EpollDispatcher::run(int timeout_ms) {
    running_ = true;
    INF("EpollDispatcher started with timeout: {} ms", timeout_ms);
    
    while (running_) {
        int num_events = epoll_wait(epoll_fd_, ready_events_.data(), max_events_, timeout_ms);
        
        if (num_events == -1) {
            if (errno == EINTR) {
                continue;  // 被信号中断，继续
            }
            ERR("epoll_wait error: {}", strerror(errno));
            break;
        }
        
        if (num_events == 0) {
            // 超时，可以在这里处理超时逻辑
            continue;
        }
        
        // 处理就绪事件
        for (int i = 0; i < num_events; ++i) {
            auto* event_ptr = static_cast<Event*>(ready_events_[i].data.ptr);
            if (event_ptr) {
                event_ptr->handle_event(static_cast<EventType>(ready_events_[i].events));
            }
        }
    }
    
    INF("EpollDispatcher stopped");
}

void EpollDispatcher::cleanup() {
    if (epoll_fd_ != -1) {
        close(epoll_fd_);
        epoll_fd_ = -1;
    }
    events_.clear();
    ready_events_.clear();
}