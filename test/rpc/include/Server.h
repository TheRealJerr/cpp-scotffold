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
#include "command.h"
#include "../include/log.h"
#include "../include/Buffer.h"
using namespace Tools;

class Server {
private:
    Reactor reactor_;
    int server_fd_;
    
    bool _bind_and_listen(int port);

public:
    Server() : server_fd_(-1) {}
    
    bool start(int port);
    
    void stop();
    
private:
    void handleAccept(int fd, EventType type);
    
    void handleClientData(int client_fd, EventType type);
    
    void handleClientWrite(int client_fd, buffer_ptr recv_buffer, buffer_ptr send_buffer);
    
};
