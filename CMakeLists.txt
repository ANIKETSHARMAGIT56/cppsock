cmake_minimum_required(VERSION 3.30)
project(cppsock)

set(CMAKE_CXX_STANDARD 20)
add_library(cppsock
        src/ws_impl/boost_server.cpp
        src/protocol/protocol.cpp
        )
target_include_directories(cppsock PUBLIC include)
add_subdirectory(deps/glaze)

target_link_libraries(cppsock PRIVATE glaze::glaze)
add_subdirectory(examples/raw_websocket_server)