cmake_minimum_required(VERSION 3.15)
project(CppCoroExamples LANGUAGES CXX)

# 设置编译器和C++标准
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 包含vcpkg的头文件路径
include_directories("/home/dev/vcpkg/installed/x64-linux/include")

# 添加可执行文件
add_executable(cppcoro_demo src/main.cpp)

# 链接cppcoro库
target_link_libraries(cppcoro_demo "/home/dev/vcpkg/installed/x64-linux/lib/libcppcoro.a")

# 设置编译选项
target_compile_options(cppcoro_demo PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# 如果使用 find_package 方式，可以这样写：
# find_package(cppcoro CONFIG REQUIRED)
# target_link_libraries(cppcoro_demo PRIVATE cppcoro::cppcoro)