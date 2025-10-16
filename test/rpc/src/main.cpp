#include "../include/Server.h"
#include "../include/test.pb.h"
using namespace rpc;

int main()
{

    init_global_logging();
    std::shared_ptr<Server> svr = std::make_shared<Server>();

    svr->set_on_connect_callback([](int fd, const std::string& ip, int port) { 
        INF("fd: {}, ip: {}, port: {}", fd, ip, port);
        INF("success to connect and use the handler");
    });

    svr->set_on_msg_callback([](int fd, buffer_ptr recv, buffer_ptr send){
        // 设置一个简单的http html文件
        // recv_buffer是一个protobuf的字符串   
        Request req;
        req.ParseFromString(recv->readAllString());

        INF("num1 : {}, num2 : {}", req.num1(), req.num2());

        // 进行业务计算
        Response res;
        res.set_result(req.num1() + req.num2());



        send->append(res.SerializeAsString());

    });

    svr->start(9000);

    getchar();

    
    return 0;
}