// 

#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
namespace lg = spdlog;

// 定义标准输出
void StdOutLog()
{
    auto console = lg::stdout_color_mt("console");
    console->set_level(lg::level::err);
    console->info("Welcome to spdlog!");
    console->debug("This is a debug message");
    console->error("Some error message here");
}
int main()
{
    StdOutLog();
    return 0;
}