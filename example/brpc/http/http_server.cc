
#include <brpc/server.h>
#include <brpc/http2.h>
#include <brpc/controller.h>
#include <brpc/closure_guard.h>
#include "../caculate.pb.h"

using namespace caculate;

class CaculateServiceImpl : public CaculateService {

public:
    // 添加服务的接口
    virtual void Hello(google::protobuf::RpcController* controller,
                    const HelloRequest* request,
                    HelloResponse* response,
                    google::protobuf::Closure* done) override
    {
        brpc::ClosureGuard done_guard(done);
        auto* cntl = dynamic_cast<brpc::Controller*>(controller);

        const auto& headers = cntl->http_request();
        std::cout << "Method: " << brpc::HttpMethod2Str(headers.method()) << std::endl;
        std::cout << "Path: " << cntl->request_attachment().to_string() << std::endl;

        cntl->response_attachment().append("Hello World from HTTP!");
        cntl->http_response().set_content_type("text/plain");
        cntl->http_response().set_status_code(200);

    }

    // 注册同步的Add方法
    virtual void Add(google::protobuf::RpcController* controller,
                    const AddRequest* request,
                    AddResponse* response,
                    google::protobuf::Closure* done) override
    {
        brpc::ClosureGuard done_guard(done);

        int result = request->num1() + request->num2();

        response->set_result(result);
    }
};

int main()
{
    
    /*
        1. 实例化计算服务对象
        2. 实例化Server对象
        3. 注册对应的服务
    */
    
    CaculateServiceImpl service;
    
    brpc::ServerOptions options;

    

    options.idle_timeout_sec = -1;
    
    brpc::Server server;

    int ret = server.AddService(&service, brpc::SERVER_DOESNT_OWN_SERVICE);
    if(ret == -1)
    {
        std::cout << "Add Service Failed" << std::endl;
    
        return -1;
    }

    ret = server.Start(9000, &options);

    if(ret == -1)
    {
        std::cout << "Start Server Failed" << std::endl;
        return -1;
    }
    
    server.RunUntilAskedToQuit();


    return 0;
}