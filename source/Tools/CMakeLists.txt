cmake_minimum_required(VERSION 3.10)

project(test_etcd)

set(CMAKE_CXX_STANDARD 17)

# 包含头文件目录
include_directories(include)
# 查找公共的源文件（src目录下的文件）
file(GLOB_RECURSE COMMON_SOURCES src/*.cpp src/*.cc test.pb.cc)

# 查找依赖库
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(gflags REQUIRED)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 公共的链接库列表
set(COMMON_LIBS 
    spdlog::spdlog 
    fmt::fmt 
    pthread 
    gflags 
    avcodec 
    avformat 
    avutil 
    swscale 
    swresample 
    jsoncpp 
    etcd-cpp-api 
    cpprest
    brpc 
    protobuf
    leveldb
    ssl 
    crypto 
    dl
)

# 两个服务的提供者

# 创建 provider1 可执行程序
add_executable(provider1 test/provider1.cc ${COMMON_SOURCES})
target_link_libraries(provider1 ${COMMON_LIBS})
set_target_properties(provider1 PROPERTIES OUTPUT_NAME provider1)

# 创建 provider2 可执行程序
add_executable(provider2 test/provider2.cc ${COMMON_SOURCES})
target_link_libraries(provider2 ${COMMON_LIBS})
set_target_properties(provider2 PROPERTIES OUTPUT_NAME provider2)

# 服务发现
add_executable(discovery test/discovery.cpp ${COMMON_SOURCES})
target_link_libraries(discovery ${COMMON_LIBS})
set_target_properties(discovery PROPERTIES OUTPUT_NAME discovery)