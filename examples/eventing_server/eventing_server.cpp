#include <iostream>
#include <thread>
#include <boost/core/data.hpp>

#include <cppsock/ws_impl/boost_server.h>


#include <cppsock/protocol/protocol.h>

int main() {
    auto ws_Server =std::make_shared<cppsock::ws::ws_server_boost_impl>();
    cppsock::eventing::event_manager event_server(std::move(ws_Server),{1,"server","okay"});
    event_server.on_event("testing event", [](cppsock::eventing::event ev) {
        std::cout << ev << std::endl;
    });

    event_server.run(1831, 1);
}
