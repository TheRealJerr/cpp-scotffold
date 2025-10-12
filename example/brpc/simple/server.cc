#include <brpc/server.h>
#include <brpc/log.h>
#include <butil/logging.h>
#include "caculate.pb.h"
#include <thread>
#include "rigistry.h"

using namespace caculate;

// 关闭日志服务
class CaculateServiceImpl : public CaculateService {

public:

    virtual void Add(google::protobuf::RpcController* controller,
                    const AddRequest* request,
                    AddResponse* response,
                    google::protobuf::Closure* done) override
    {
        std::thread([=](){
            brpc::ClosureGuard done_guard(done);

            int result = request->num1() + request->num2();

            response->set_result(result);

            std::cout << "caculate finished" << std::endl;
        }).detach();

    }
};


int main()
{
    /*
        1. 实例化计算服务对象
        2. 实例化Server对象
        3. 注册对应的服务
    */
    CaculateServiceImpl *service = new CaculateServiceImpl();
    
    
    auto server = RpcTools::RpcServerFactory::create_server(service, 9000);
    
    
    server->RunUntilAskedToQuit();

    return 0;
}


