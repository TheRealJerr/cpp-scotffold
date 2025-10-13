// reactor.cpp
#include "../include/reactor.h"
#include <iostream>

Reactor::Reactor() : running_(false) {}

Reactor::~Reactor() {
    stop();
}

void Reactor::start() {
    if (running_) {
        return;
    }
    
    running_ = true;
    worker_thread_ = std::thread(&Reactor::run, this);
    std::cout << "Reactor started" << std::endl;
}

void Reactor::stop() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    dispatcher_.stop();
    
    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
    
    std::cout << "Reactor stopped" << std::endl;
}

bool Reactor::registerEvent(std::shared_ptr<Event> event) {
    return dispatcher_.add_event(std::move(event));
}

bool Reactor::unregisterEvent(int fd) {
    return dispatcher_.delete_event(fd);
}

bool Reactor::updateEvent(std::shared_ptr<Event> event) {
    return dispatcher_.modify_event(std::move(event));
}

void Reactor::run() {
    dispatcher_.run(-1);  // 无超时
}