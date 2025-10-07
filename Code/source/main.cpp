#include "log.h"
#include <gflags/gflags.h>

DEFINE_bool(log_async, true, "是否启用异步日志");
DEFINE_string(log_target, "stdout", "日志输出目标，可选值：stdout、filem、console");
DEFINE_int32(log_level, 1, "日志级别，0-trace, 1-debug, 2-info, 3-warn, 4-err");
DEFINE_string(log_pattern, "[%H:%M:%S][%-7l]%v", "日志的输出格式");

int main(int argc, char* argv[])
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    // 初始化日志器的参数
    struct Tools::log_config config = {
       .async_ = FLAGS_log_async,
       .level_ = FLAGS_log_level,
       .pattern_ = FLAGS_log_pattern,
       .target_ = FLAGS_log_target
    };

    // 初始化日志器
    Tools::init_log(config);

    DBG("Hello, world!");

    return 0;
}