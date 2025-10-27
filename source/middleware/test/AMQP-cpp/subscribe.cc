#include <iostream>
#include "../../include/AMQP.h"
#include "../../include/log.h"
#include "../../include/command.hpp"
const std::string rabbitmq_url = "amqp://admin:123456@192.168.124.148:5672/";

int main()
{
    init_global_logging();
    Tools::client_ptr_t client = std::make_shared<Tools::AsyncAMQPClient>(rabbitmq_url);

    client->start();
    Tools::DeclareConfig config = 
    {
        .ex_name = "ex_name",
        .ex_type = Tools::ExchangeType::DIRECT,
        .que_name = "que_name",
        .bind_key = "bind_key",
        .ttl = 0
    };
    Tools::AMQPSubscribe subscribe(client, config);

    auto on_message = [](const char* msg, size_t length)
        {
            INF("Received message: {}", std::string(msg, length));
        };
    subscribe.consume(on_message);

    getchar();
    return 0;
}