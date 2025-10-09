/*
    gflags的测试的文件
*/

#include <iostream>
#include <gflags/gflags.h>
#include <string>
#include <cmath>

// 定义捕获的参数信息
DEFINE_bool(reuse, true, "reuse port");
DEFINE_string(ip, "127.0.0.1", "ip address");
DEFINE_int32(port, 8080, "port number");
DEFINE_string(listen_ip, "0.0.0.0", "listen ip address");

// 定义主函数
int 
main(int argc, char** argv)
{
    // 解析参数
    google::ParseCommandLineFlags(&argc, &argv, true);
    // 访问参数
    std::cout << "reuse: " << FLAGS_reuse << std::endl;
    std::cout << "ip: " << FLAGS_ip << std::endl;
    std::cout << "port: " << FLAGS_port << std::endl;
    std::cout << "listen_ip: " << FLAGS_listen_ip << std::endl;
    // 解析参数的另一种方式
    
    return 0;
}

/*
    ./main --listen_ip=127.0.0.1 --port=22 --reuse=false
*/