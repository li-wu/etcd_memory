cmake_minimum_required(VERSION 3.20)
project(test_etcd)

set(CMAKE_CXX_STANDARD 17)

find_package(etcd-cpp-api REQUIRED)

add_executable(test_etcd memory_leak.cpp)

target_link_libraries(test_etcd
        PUBLIC
        gRPC::gpr etcd-cpp-api gRPC::grpc++
        )

