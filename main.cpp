#include <iostream>
#include <thread>

#include <cppsock/ws_impl/boost_server.h>

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



int main() {
    cppsock::ws::ws_server_boost_impl smth;
            smth.on_connect([&](size_t) {
                std::cout << "connected" << std::endl;
            })
            .on_message([&](size_t client_id, const std::string &message) {
                std::vector<char> data(message.begin(), message.end());
                if (data.at(0)==1) {
                    smth.send(client_id, "connect packet recieved");
                    if(data.at(1)!=0) {
                        smth.send(client_id, "payload detected");
                        if (data.size()==data.at(1)+2) {
                            std::cout << "payload detected properly" << std::endl;
                        }
                        else {
                            std::cout << "payload detected improperly" << std::endl;
                        }
                    }
                }
            })
            .on_disconnect([&](size_t, size_t) {
                std::cout << "disconnected" << std::endl;
            });
    std::thread([&smth]() {
            smth.run(1831, 1);
        }
    ).detach();
    while (true) {
        scanf("sd");
        smth.close(0, 1000, "closed");
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
