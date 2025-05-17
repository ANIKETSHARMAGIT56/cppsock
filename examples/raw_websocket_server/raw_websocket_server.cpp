#include <iostream>
#include <thread>
#include <boost/core/data.hpp>
#include <cppsock/ws_impl/boost_server.h>
#include <cppsock/protocol/protocol.h>

int main() {
    
    cppsock::ws::ws_server_boost_impl ws_server;
    ws_server
    .on_connect([&](size_t client_id){
        std::cout << "client connected with id: " <<client_id<< std::endl;
    })
    .on_message([&](size_t client_id , std::string message_data){
        std::cout << "client " <<client_id<< " sent: "<< message_data<< std::endl;
    })
    .on_disconnect([&](size_t client_id, cppsock::DISCONNECT_REASON){
        std::cout << "client "<< client_id<<" disconnected";
    });
    ws_server.run(1831, 1);

}
