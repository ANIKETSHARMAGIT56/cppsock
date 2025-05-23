#include <iostream>
#include <thread>
#include <boost/core/data.hpp>

#include <cppsock/ws_impl/boost_server.h>


#include <cppsock/protocol/protocol.h>

int main() {
    auto ws_Server =std::make_shared<cppsock::ws::ws_server_boost_impl>();
    auto event_server= std::make_shared<cppsock::eventing::event_manager>(std::move(ws_Server),cppsock::eventing::eventing_peer{1,"hi","dfe"});


    event_server->on_event("testing event", [](cppsock::eventing::event ev) {
        std::cout << ev << std::endl;
    });

    event_server->run(1831, 1);
}
