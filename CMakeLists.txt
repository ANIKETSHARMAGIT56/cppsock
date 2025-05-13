cmake_minimum_required(VERSION 3.30)
project(cppsock)

set(CMAKE_CXX_STANDARD 20)
add_executable(cppsock
        src/ws_impl/boost_server.cpp
        src/protocol/protocol.cpp
        main.cpp)
target_include_directories(cppsock PUBLIC include)
