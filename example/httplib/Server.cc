#include <httplib.h>

// http的请求格式

/*
    方法   路径   版本
    GET /index.html HTTP/1.1
    空行
    请求头
    Content-Type: text/html
    Content-Length: 100
    空行
    请求体
*/
void hello_handler(const httplib::Request& req, httplib::Response& res)
{
    std::cout << req.method << std::endl;
    std::cout << req.path << std::endl;
    std::cout << req.version << std::endl;
    std::cout << req.body << std::endl;

    std::string html_body = "<html><body><h1>Hello, World!</h1></body></html>";
    res.set_content(html_body, "text/html");
    res.set_header("Content-Length", std::to_string(html_body.length()));
    res.set_header("Content-Type", "text/html");
    res.status = 200;
}
int main()
{
    httplib::Server svr;
    // 注册GET请求的处理函数
    svr.Get("/hello", hello_handler);
    svr.listen("0.0.0.0", 9000);
    
    return 0;
}