#include <iostream>
#include <thread>
#include <boost/core/data.hpp>

#include <cppsock/ws_impl/boost_server.h>

#include "glaze/reflection/get_name.hpp"

enum packet_type {
    message     = 0,
    connect     = 1,
    disconnect  = 2,
    ping        = 3,
    pong        = 4,
};

struct packet {
    size_t type;
    std::string payload;
};
uint32_t fnv1a_hash(const char* str) {
    uint32_t hash = 2166136261u; // FNV offset basis
    while (*str) {
        hash ^= (uint8_t)*str++; // XOR with byte, then shift
        hash *= 16777619u; // FNV prime multiplier
    }
    return hash;
}

bool parse_packet(std::string &packet_string,packet &packet) {
    std::vector<char> data(packet_string.begin(), packet_string.end());

}


#include <cppsock/protocol/protocol.h>

int main() {
//
//     auto ev2= cppsock::eventing::event_deserialize(strin);


    // cppsock::ws::ws_server_boost_impl smth;
    //         smth.on_connect([&](size_t) {
    //             // std::cout << "connected" << std::endl;
    //         })
    //         .on_message([&](size_t client_id, const std::string &message) {
    //             std::vector<char> data(message.begin(), message.end());
    //             std::cout << "message recieved" << std::endl;
    //             std::string packet_payload = std::string(data.begin() + 1, data.end());
    //             smth.send(client_id, packet_payload);
    //         })
    //         .on_disconnect([&](size_t, size_t) {
    //             std::cout << "disconnected" << std::endl;
    //         });
    // std::thread([&smth]() {
    //         smth.run(1831, 1);
    //     }
    // ).detach();
    // while (true) {
    //     scanf("sd");
    //     smth.close(0, 1000, "closed");
    //     std::this_thread::sleep_for(std::chrono::seconds(1));
    // }

    auto ws_Server =std::make_shared<cppsock::ws::ws_server_boost_impl>();
    auto event_server= std::make_shared<cppsock::eventing::event_manager>(std::move(ws_Server),cppsock::eventing::eventing_peer{1,"hi","dfe"});


    event_server->on_event("testing event", [](cppsock::eventing::event ev) {
        std::cout << ev << std::endl;
    });

    event_server->run(1831, 1);
}
