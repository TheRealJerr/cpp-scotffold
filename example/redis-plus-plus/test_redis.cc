#include <sw/redis++/redis++.h>
#include <memory>
#include <gflags/gflags.h>
#include <iostream>
#include <string>
// 通过gflags设置Redis的连接信息
DEFINE_string(redis_host, "localhost", "Redis host");
DEFINE_int32(redis_port, 6379, "Redis port");

using redis_client_t = std::shared_ptr<sw::redis::Redis>;

const std::string redis_translation_protovol = "tcp";

// 创建Redis客户端
redis_client_t create_redis_client(const std::string& host, int port)
{
    auto command = redis_translation_protovol + "://" + host + ":" + std::to_string(port);
    redis_client_t client = std::make_shared<sw::redis::Redis>(command);
    return client;
}

// 测试Redis客户端
void test_redis_client(redis_client_t& client)
{
    // 测试Redis的常见的功能
    client->set("key", "value");
    auto opt_str = client->get("key");
    if(opt_str.has_value())
        std::cout << "value: " << opt_str.value() << std::endl;
    client->del("key");
}


int main(int argc, char* argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, false);
    // 创建Redis客户端
    auto redis = create_redis_client(FLAGS_redis_host, FLAGS_redis_port);
    // 测试Redis客户端
    test_redis_client(redis);
    return 0;
}
