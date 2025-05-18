project(eventing_server)
add_executable(eventing_server eventing_server.cpp)
target_link_libraries(eventing_server PUBLIC cppsock glaze::glaze)