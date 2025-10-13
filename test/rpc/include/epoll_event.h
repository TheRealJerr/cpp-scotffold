// event.h
#ifndef EVENT_H
#define EVENT_H

#include <functional>
#include <memory>
#include <unistd.h>
#include <sys/epoll.h>

class EpollDispatcher;


#include <type_traits>
#include <iostream>

// 使用 std::underlying_type_t 获取底层类型
enum class EventType : int64_t {
    NONE        = 0,
    READ        = EPOLLIN,      // 可读事件
    WRITE       = EPOLLOUT,     // 可写事件  
    ERROR       = EPOLLERR,     // 错误事件
    HANGUP      = EPOLLHUP,     // 挂起事件
    EDGE_TRIGGER = EPOLLET,     // 边缘触发模式
    ONESHOT     = EPOLLONESHOT  // 一次性事件
};

// 位运算运算符重载
inline constexpr EventType operator|(EventType lhs, EventType rhs) {
    using underlying = std::underlying_type_t<EventType>;
    return static_cast<EventType>(
        static_cast<underlying>(lhs) | static_cast<underlying>(rhs)
    );
}

inline constexpr EventType operator&(EventType lhs, EventType rhs) {
    using underlying = std::underlying_type_t<EventType>;
    return static_cast<EventType>(
        static_cast<underlying>(lhs) & static_cast<underlying>(rhs)
    );
}

inline constexpr EventType operator^(EventType lhs, EventType rhs) {
    using underlying = std::underlying_type_t<EventType>;
    return static_cast<EventType>(
        static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs)
    );
}

inline constexpr EventType operator~(EventType value) {
    using underlying = std::underlying_type_t<EventType>;
    return static_cast<EventType>(~static_cast<underlying>(value));
}

inline EventType& operator|=(EventType& lhs, EventType rhs) {
    lhs = lhs | rhs;
    return lhs;
}

inline EventType& operator&=(EventType& lhs, EventType rhs) {
    lhs = lhs & rhs;
    return lhs;
}

inline EventType& operator^=(EventType& lhs, EventType rhs) {
    lhs = lhs ^ rhs;
    return lhs;
}

// 事件回调函数类型
using EventCallback = std::function<void(int fd, EventType type, void* user_data)>;

// 事件基类
class Event {
public:
    Event(int fd, EventType type, EventCallback callback, void* user_data = nullptr);
    virtual ~Event() = default;
    
    int get_fd() const { return fd_; }
    EventType get_type() const { return type_; }
    void* get_user_data() const { return user_data_; }
    EventCallback get_callback() const { return callback_; }
    
    void set_user_data(void* user_data) { user_data_ = user_data; }
    void set_callback(const EventCallback& callback) { callback_ = callback; }
    
    // 转换为 epoll_event 结构
    epoll_event transfrom_to_epoll_event() const;
    
    // 处理事件
    virtual void handle_event(EventType events);

protected:
    int fd_;
    EventType type_;
    EventCallback callback_;
    void* user_data_;
};

#endif // EVENT_H