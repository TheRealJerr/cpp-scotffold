cmake_minimum_required(VERSION 4.0)


project(folly)

# C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# 添加链接的库
find_package(gflags REQUIRED)