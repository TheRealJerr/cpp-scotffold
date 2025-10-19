
#include <etcd.h>

const std::string etcd_addr = "http://127.0.0.1:2379";

void test_registry() {
    std::shared_ptr<Tools::ServiceProvider> provider
        = std::make_shared<Tools::ServiceProvider>(etcd_addr);
    // 注册服务
    provider->register_service("127.0.0.1:9000", "test_method");

    INF("回车退出");
    getchar();
    provider->deregister_service();
}

int main()
{
    init_global_logging();
    test_registry();
    return 0;
}