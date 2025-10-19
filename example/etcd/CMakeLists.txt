cmake_minimum_required(VERSION 3.10)
project(etcd_test)

set(CMAKE_CXX_STANDARD 17)

include_directories(include /usr/include)

file(GLOB_RECURSE SOURCES src/*.cpp src/*.cc)

add_executable(etcd_test ${SOURCES})

# 直接链接库
target_link_libraries(etcd_test etcd-cpp-api pthread cpprest ssl crypto dl)

set_target_properties(etcd_test PROPERTIES OUTPUT_NAME etcd_test)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)