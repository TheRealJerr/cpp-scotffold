#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/endpoint.hpp>
#include <websocketpp/server.hpp>


using websocket_server_t = websocketpp::server<websocketpp::config::asio>;

// 注册握手成功的回调函数
auto on_open_hdl = [](websocket_server_t* svr, websocketpp::connection_hdl hdl) -> void {
    
    
    // 获取连接对象的基本的信息
    std::cout << "连接建立成功" << std::endl;
};

// 定义收到消息的回调函数
auto on_msg_hdl = [](websocket_server_t* svr, websocketpp::connection_hdl hdl, websocket_server_t::message_ptr msg) -> void
{

    std::cout << "收到消息: " << msg->get_payload() << std::endl;

    auto con = svr->get_con_from_hdl(hdl);

    // 
    // 发送消息
    con->send("回显" + msg->get_payload());
};

auto on_close_hdl = [](websocket_server_t* svr, websocketpp::connection_hdl hdl) -> void {
    std::cout << "连接断开" << std::endl;

    // 关闭连接

    auto con = svr->get_con_from_hdl(hdl);
    con->close(websocketpp::close::status::normal, "关闭连接");
    
};
// 定义http请求的回调函数
auto on_http_hdl = [](websocket_server_t* svr, websocketpp::connection_hdl hdl) -> void {

    auto con = svr->get_con_from_hdl(hdl);

    // 打印http请求信息
    // 方法 + 路径 + 版本
    auto& req = con->get_request();
    std::cout << "收到HTTP请求: " << req.get_method() << " " << req.get_uri() << " " << req.get_version() << std::endl;
    // 获取请求头
    auto headers = req.get_headers();
    for (auto& header : headers) {
        std::cout << header.first << ": " << header.second << std::endl;
    }
    // 获取请求内容
    auto content = req.get_body();
    std::cout << "请求内容: " << content << std::endl;
    // 构造相应的信息
    // 发送响应
    con->set_status(websocketpp::http::status_code::ok);
    con->set_body("<html><body>Hello, world!</body></html>");
    con->append_header("Content-Type", "text/html");

};
void test_websocketpp()
{
    /*
        1. 创建一个服务器节点
        2. 注册对应的回调函数, 握手成功, 连接断开, 收到消息等
        3. 初始化asio框架
        4. 启动地址重用
        5. bind地址和端口
        6. 启动服务
    */

    websocketpp::server<websocketpp::config::asio> server;

   // 关闭日志
   server.clear_access_channels(websocketpp::log::alevel::all);

    
    server.set_open_handler(std::bind(on_open_hdl, &server, std::placeholders::_1));
    
    server.set_message_handler(std::bind(on_msg_hdl, &server, std::placeholders::_1, std::placeholders::_2));
    // 定义连接断开的回调函数
    server.set_close_handler(std::bind(on_close_hdl, &server, std::placeholders::_1));
    // 注册http的回调函数
    server.set_http_handler(std::bind(on_http_hdl, &server, std::placeholders::_1));
    
    server.set_close_handler(std::bind(on_close_hdl, &server, std::placeholders::_1));
    // 初始化Asio框架
    server.init_asio();
    // 设置地址重用
    server.set_reuse_addr(true);
    // 绑定地址和端口
    server.listen(9000);
    // 启动服务
    server.start_accept();

    server.run();
}

int main(int argc,char* argv[])
{
    // 测试websocketpp库
    test_websocketpp();
    return 0;
}