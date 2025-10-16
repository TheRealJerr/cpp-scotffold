# 帮我编译一个CMakeLists.txt的文件, 头文件在include/, 源文件在src/, 输出文件在bin, 链接spdlog, fmt库

cmake_minimum_required(VERSION 3.10)

project(test_ffmpeg)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

# 这里修改一下, 应该是src/*cpp
file(GLOB_RECURSE SOURCES src/*.cpp src/*.cc)

add_executable(test_ffmpeg ${SOURCES})

find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(gflags REQUIRED)

target_link_libraries(test_ffmpeg spdlog::spdlog fmt::fmt pthread gflags avcodec avformat avutil swscale swresample jsoncpp)

set_target_properties(test_ffmpeg PROPERTIES OUTPUT_NAME test_ffmpeg)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

