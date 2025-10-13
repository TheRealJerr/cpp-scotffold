#pragma once
#include "epoll_event.h"

// concrete_events.h
#ifndef CONCRETE_EVENTS_H
#define CONCRETE_EVENTS_H

#include "event.h"
#include <functional>
#include "Buffer.h"
#include <event.h>
// 定时器事件
class TimerEvent : public Event {
public:
    using TimeoutCallback = std::function<void()>;
    
    TimerEvent(int fd, TimeoutCallback callback);
    void handle_event(EventType events) override;

private:
    TimeoutCallback timeout_callback_;
};

// IO 事件
class IOEvent : public Event {
public:
    using IOCallback = std::function<void(int fd, EventType type)>;
    
    IOEvent(int fd, EventType type, IOCallback callback, void* user_data = nullptr);
    void handle_event(EventType events) override;

private:
    IOCallback io_callback_;
};

// 信号事件
class SignalEvent : public Event {
public:
    using SignalCallback = std::function<void(int signum)>;
    
    SignalEvent(int fd, SignalCallback callback);
    void handle_event(EventType events) override;

private:
    SignalCallback signal_callback_;
};

// 网络时间
class NetworkEvent : public Event
{
public:

    using NetworkCallback = std::function<void(int fd, EventType type, buffer_ptr recv_buffer,buffer_ptr send_buffer)>;

    NetworkEvent(int fd, EventType type,
        NetworkCallback callback, 
        void* user_data = nullptr);

    void handle_event(EventType events) override;

    void set_recv_buffer(buffer_ptr recv_buffer) { recv_buffer_ = recv_buffer; }

private:
    int fd_;
    NetworkCallback network_callback_;
    buffer_ptr recv_buffer_;
    buffer_ptr send_buffer_;
};

// 创建时间对象的工厂类

class EventFactory
{
public:
    template <class EventType,class ...Args>
    static std::shared_ptr<EventType> create(Args&&... args)
    {
        return std::make_shared<EventType>(std::forward<Args>(args)...);
    }
};  

#endif // CONCRETE_EVENTS_H