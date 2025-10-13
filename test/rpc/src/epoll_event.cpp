#include "../include/epoll_dispatcher.h"


Event::Event(int fd, EventType type, EventCallback callback, void* user_data)
    : fd_(fd), type_(type), callback_(std::move(callback)), user_data_(user_data) {}

epoll_event Event::transfrom_to_epoll_event() const {
    epoll_event ev;
    ev.events = static_cast<uint32_t>(type_);
    ev.data.fd = fd_;
    return ev;
}

void Event::handle_event(EventType events) {
    if (callback_) {
        callback_(fd_, events, user_data_);
    }
}