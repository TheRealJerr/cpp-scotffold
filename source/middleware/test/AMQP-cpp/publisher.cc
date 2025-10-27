#include <iostream>
#include "../../include/AMQP.h"
#include "../../include/log.h"
#include "../../include/command.hpp"
const std::string rabbitmq_url = "amqp://admin:123456@192.168.124.148:5672/";

int main()
{
    init_global_logging();
    Tools::client_ptr_t client = std::make_shared<Tools::AsyncAMQPClient>(rabbitmq_url);
    // 启动事件循环
    client->start();
    Tools::DeclareConfig config = 
    {
        .ex_name = "ex_name",
        .ex_type = Tools::ExchangeType::DELAYED,
        .que_name = "que_name",
        .bind_key = "bind_key",
        .ttl = 0
    };
    Tools::AMQPPublish publish(client, config);
    publish.publish("hello world");

    getchar();
    return 0;
}