#include <httplib.h>

using namespace httplib;

int main()
{
    Client cli("localhost", 8080);
    auto res = cli.Get("/hello");
    if (res) {
        std::cout << res->body << std::endl;
    }
    return 0;
}