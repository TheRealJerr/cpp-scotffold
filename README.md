# cpp-scotffold

基于C++的微服务脚手架系统

## 中间件

- **gflags**: 命令行参数解析库
- **gtest**: 单元测试框架
- **spdlog**: 日志库
- **jsoncpp**: json解析库
- **protobuf**: 协议缓冲区库
- **brpc**: 百度的RPC框架
- **httplib**: HTTP服务器库
- **websocketpp**: WebSocket服务器库
- **mysql-xdevapi**: MySQL数据库驱动库
- **redis++**: Redis数据库驱动库


## 依赖管理

这个项目使用cmake作为构建系统，依赖管理使用cmake的find_package命令。

## 目录结构 ./source

- json.h : 
    实现基于jsoncpp的json序列化和反序列化
- log.h : 
    实现基于spdlog的日志系统, 使得可以使用glibc++中的`__FILE`和`__LINE__`宏
- command.h
    封装了常见的参数的信息, 包括全局日志器的启动选项, 以及命令行参数的解析