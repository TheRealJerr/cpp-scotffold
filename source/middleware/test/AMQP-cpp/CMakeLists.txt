cmake_minimum_required(VERSION 3.10)
project(MyProject)

# 设置C++标准为C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 添加头文件搜索路径
include_directories(../../include)


# 生成publisher可执行文件
add_executable(publisher dq_publisher.cc ../../src/command.cpp  ../../src/log.cpp ../../src/AMQP.cpp)

# 生成subscriber可执行文件
add_executable(subscriber dq_subscribe.cc ../../src/command.cpp  ../../src/log.cpp ../../src/AMQP.cpp)

# 链接库
target_link_libraries(publisher 
    pthread 
    amqpcpp 
    ev 
    gflags 
    spdlog 
    leveldb 
    dl 
    ssl 
    crypto 
    fmt
)

target_link_libraries(subscriber 
    pthread 
    amqpcpp 
    ev 
    gflags 
    spdlog 
    leveldb 
    dl 
    ssl 
    crypto 
    fmt
)

# 如果publisher.cc和subscriber.cc在同一个目录下，可以简化为：
# add_executable(publisher publisher.cc)
# add_executable(subscriber subscriber.cc)
# target_link_libraries(publisher ${LIBS})
# target_link_libraries(subscriber ${LIBS})
# set(LIBS pthread amqpcpp ev gflags spdlog leveldb dl ssl crypto fmt)
