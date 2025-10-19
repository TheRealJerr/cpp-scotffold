#include <etcd.h>

const std::string etcd_addr = "http://127.0.0.1:2379";

void online_callback(const std::string& service_name, const std::string& endpoint)
{
    INF("{}服务上线了, endpoint={}", service_name, endpoint);
}

void offline_callback(const std::string& service_name, const std::string& endpoint)
{
    INF("{}服务下线了, endpoint={}", service_name, endpoint);
}



void test_etcd_discovery()
{   
    // 创建一个服务的发现者
    auto discovery = std::make_shared<Tools::ServiceDiscovery>(etcd_addr, online_callback, offline_callback);

    discovery->watch_service();

    INF("等待服务上线...");
    getchar();
}
int main()
{
    init_global_logging();
    test_etcd_discovery();
    return 0;
}