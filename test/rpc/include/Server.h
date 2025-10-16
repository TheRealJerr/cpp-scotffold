// example.cpp
#pragma once
#include "../include/reactor.h"
#include "../include/concrete_events.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <sys/timerfd.h>
#include <cstring>
#include <iostream>
#include "command.h"
#include "../include/log.h"
#include "../include/Buffer.h"
using namespace Tools;

class Server : public std::enable_shared_from_this<Server> {
private:
    using on_msg_callback = std::function<void(int, buffer_ptr, buffer_ptr)>;

    using on_connect_callback = std::function<void(int,const std::string&, int)>;

    Reactor reactor_;
    int server_fd_;
    // 数据处理回调函数
    on_msg_callback on_msg_callback_;
    // 连接建立回调函数
    on_connect_callback on_connect_callback_;
    
    bool _bind_and_listen(int port);

public:
    Server() : server_fd_(-1) {}
    
    bool start(int port);
    
    void stop();

    void set_on_msg_callback(on_msg_callback callback) {
        on_msg_callback_ = callback;
    }

    void set_on_connect_callback(on_connect_callback callback) {
        on_connect_callback_ = callback;
    }
    
private:
    void handleAccept(int fd, EventType type);
    
    void handleClientData(int client_fd, EventType type);
    
    void handleClientWrite(int client_fd, buffer_ptr recv_buffer, buffer_ptr send_buffer);
    
};
