# cpp-scotffold

基于C++的微服务脚手架系统

## 依赖库

- **gflags**: 命令行参数解析库
- **gtest**: 单元测试框架
- **spdlog**: 日志库
- **jsoncpp**: json解析库
- **protobuf**: 协议缓冲区库
- **brpc**: 百度的RPC框架
- **httplib**: HTTP服务器库
- **websocketpp**: WebSocket服务器库
- **mysql-xdevapi**: MySQL数据库驱动库
- **redis++**: redis客户端库
- **ffmpeg**: 视频处理库



## 目录结构 

- source/util.h : 
    - json的序列化和反序列化
    - 文件资源的接口类
- source/log.h : 
    实现基于spdlog的日志系统, 使得可以使用glibc++中的`__FILE`和`__LINE__`宏
- source/command.h
    封装了常见的参数的信息, 包括全局日志器的启动选项, 以及命令行参数的解析
- example
    示例代码, 包括一个简单的http服务器, 一个简单的websocket服务器, 以及一个简单的mysql客户端
- source/hls.h
    基于hls协议, 实现对于ffmpeg的封装, 使得可以方便的进行hls流的处理
- source/rigistry.h
    针对brpc的channel信道管理进行封装, 通过注册中心的方式, 使得brpc的channel可以动态的进行管理
- source/redis.h
    - 通过redis++实现redis的连接池化技术
