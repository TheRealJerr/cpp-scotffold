// example.cpp
#include "../include/reactor.h"
#include "../include/concrete_events.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <sys/timerfd.h>
#include <cstring>
#include <iostream>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include "command.h"
#include "../include/log.h"
#include "../include/Buffer.h"
#include "../include/Server.h"
using namespace Tools;


bool Server::_bind_and_listen(int port)
{
    // 创建服务器 socket
    server_fd_ = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (server_fd_ == -1) {
        ERR("Failed to create socket");
        return false;
    }
    
    // 设置 SO_REUSEADDR
    int opt = 1;
    setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    // 绑定地址
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    
    if (bind(server_fd_, (sockaddr*)&addr, sizeof(addr)) == -1) {
        ERR("Failed to bind: {}", strerror(errno));
        close(server_fd_);
        return false;
    }
    
    // 监听
    if (listen(server_fd_, 128) == -1) {
        ERR("Failed to listen: {}", strerror(errno));
        close(server_fd_);
        return false;
    }

    return true;
}

bool Server::start(int port)
{
    if(_bind_and_listen(port) == false)
        return false;

    // 注册服务器 socket 事件
    auto server_event = std::make_shared<IOEvent>(
        server_fd_, 
        EventType::READ,
        [this](int fd, EventType type) { handleAccept(fd, type); }
    );
    
    reactor_.registerEvent(server_event);
// 启动反应器
    reactor_.start();
    
    INF("Server started on port {}", port);
    return true;
}

void Server::stop()
{
    reactor_.stop();
    if (server_fd_ != -1) {
        close(server_fd_);
        server_fd_ = -1;
    }
}

void Server::handleAccept(int fd, EventType type) {
    sockaddr_in client_addr{};
    socklen_t addr_len = sizeof(client_addr);
    
    int client_fd = accept4(fd, (sockaddr*)&client_addr, &addr_len, SOCK_NONBLOCK);
    if (client_fd == -1) {
        ERR("Failed to accept client: {}", strerror(errno));
        return;
    }
    
    INF("New client connected: ip : {}, port : {}", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    const std::string& ip = inet_ntoa(client_addr.sin_addr);
    const int port = ntohs(client_addr.sin_port);
    on_connect_callback_(client_fd, ip, port);
    // 为客户端注册读事件
    auto client_event = std::make_shared<IOEvent>(
        client_fd,
        EventType::READ | EventType::EDGE_TRIGGER,
        [this, self = shared_from_this()](int client_fd, EventType type) { handleClientData(client_fd, type); }
    );
    
    reactor_.registerEvent(client_event);
}

void Server::handleClientData(int client_fd, EventType type) {
    // 创建接受缓冲区
    buffer_ptr recv_buffer = BufferFactory::create();
    
    char buffer[1024];
    ssize_t bytes_read;
    // 边沿触发, 循环读取
    while ((bytes_read = read(client_fd, buffer, sizeof(buffer))) > 0) {
        // 回显数据
        // 填充接受缓冲区
        recv_buffer->append(buffer, bytes_read);
        
    }
    // 回显数据
    // 创建发送缓冲区
    buffer_ptr send_buffer = BufferFactory::create();

    // 调用处理的回调函数
    on_msg_callback_(client_fd, recv_buffer, send_buffer);
    
    if (bytes_read == 0 || (bytes_read == -1 && errno != EAGAIN)) {
        // 客户端断开或错误
        std::cout << "Client disconnected: " << client_fd << std::endl;
        
        reactor_.unregisterEvent(client_fd);

        close(client_fd);
    }
    else 
    {
        // 将client_fd注册写事件
        auto write_event = std::make_shared<IOEvent>(
            client_fd,
            EventType::WRITE | EventType::EDGE_TRIGGER,
            [this, client_fd, recv_buffer, send_buffer](int fd, EventType type) { handleClientWrite(client_fd, recv_buffer, send_buffer); }
        );
        
        reactor_.updateEvent(write_event);
    }
}

void Server::handleClientWrite(int client_fd, buffer_ptr recv_buffer, buffer_ptr send_buffer){
    // 发送数据
    auto size = send_buffer->size();
    ssize_t bytes_sent = send(client_fd, send_buffer->readAllString().c_str(), size, 0);
    if (bytes_sent == -1) {
        ERR("Failed to send data to client");
        return;
    }
    send_buffer->clear();

    // 注销写事件
    reactor_.unregisterEvent(client_fd);

    // 关闭客户端连接
    ::close(client_fd);
}