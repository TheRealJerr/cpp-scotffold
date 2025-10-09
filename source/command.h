#include <gflags/gflags.h>

// 定义命令行参数的处理
// 初始化全局日志器的相关的参数
// 日志级别、日志格式、日志输出目标、是否异步输出等
DEFINE_bool(log_async, true, "是否启用异步日志");
DEFINE_string(log_target, "stdout", "日志输出目标,可选值:stdout、filem、console");
DEFINE_int32(log_level, 1, "日志级别,0-trace, 1-debug, 2-info, 3-warn, 4-err");
DEFINE_string(log_pattern, "[%H:%M:%S][%-7l]%v", "日志的输出格式");