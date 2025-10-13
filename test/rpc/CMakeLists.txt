# 帮我编译一个CMakeLists.txt的文件, 头文件在include/, 源文件在src/, 输出文件在bin, 链接spdlog, fmt库

cmake_minimum_required(VERSION 3.10)

project(rpc)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

# 这里修改一下, 应该是src/*cpp
file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(rpc ${SOURCES})

find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(gflags REQUIRED)

target_link_libraries(rpc spdlog::spdlog fmt::fmt pthread gflags)

set_target_properties(rpc PROPERTIES OUTPUT_NAME rpc)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)



