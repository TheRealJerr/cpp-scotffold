#include "../include/concrete_events.h"


// concrete_events.cpp
#include <iostream>

// TimerEvent 实现
TimerEvent::TimerEvent(int fd, TimeoutCallback callback)
    : Event(fd, EventType::READ, nullptr), timeout_callback_(std::move(callback)) {}

void TimerEvent::handle_event(EventType events) {
    if (timeout_callback_) {
        timeout_callback_();
    }
}

// IOEvent 实现
IOEvent::IOEvent(int fd, EventType type, IOCallback callback, void* user_data)
    : Event(fd, type, nullptr, user_data), io_callback_(std::move(callback)) {}

void IOEvent::handle_event(EventType events) {
    if (io_callback_) {
        io_callback_(fd_, events);
    }
}

// SignalEvent 实现
SignalEvent::SignalEvent(int fd, SignalCallback callback)
    : Event(fd, EventType::READ, nullptr), signal_callback_(std::move(callback)) {}

void SignalEvent::handle_event(EventType events) {
    if (signal_callback_) {
        // 从文件描述符读取信号值
        int signum;
        if (::read(fd_, &signum, sizeof(signum)) == sizeof(signum)) {
            signal_callback_(signum);
        }
    }
}

NetworkEvent::NetworkEvent(int fd, EventType type, 
    NetworkCallback callback, 
    void* user_data) : 
    Event(fd, type, nullptr, user_data),
    network_callback_(std::move(callback)), 
    send_buffer_(BufferFactory::create())
{

}

void NetworkEvent::handle_event(EventType events) {
    if (network_callback_) {
        network_callback_(fd_, type_, recv_buffer_, send_buffer_);
    }
}