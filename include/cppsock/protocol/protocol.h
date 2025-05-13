//
// Created by aniket on 5/6/25.
//

#ifndef EVENT_H
#define EVENT_H
#include <boost/bimap/bimap.hpp>
#include<cppsock/ws_server_base.h>

#include "cppsock/ws_impl/boost.h"

namespace cppsock::protocol {
    class protocol {
    public:
        std::shared_ptr<cppsock::ws::ws_server_base> m_ws_server;
        boost::bimaps::bimap<size_t /*peer_id*/, size_t /*ws_client_id*/> m_peer_to_ws_client;

        size_t generate_peer_id() {
            static size_t peer_id = 0;
            return peer_id++;
        };

        enum packet_type {
            PING,
            PONG,
            MESSAGE,
            CLOSE,
            ERROR
        };

        std::pair<size_t, cppsock_str> encode_message(size_t peer_id, cppsock_str message) {
            return std::make_pair(packet_type::MESSAGE, message);
        };

        std::pair<size_t, cppsock_str> decode_message(size_t peer_id, cppsock_str message) {
        };

        void on_ws_connect(size_t ws_client_id) {
            m_peer_to_ws_client.insert(
                boost::bimaps::bimap<size_t, size_t>::value_type(generate_peer_id(), ws_client_id));
        };

        void on_ws_message(size_t ws_client_id, cppsock_str message) {
        }

        protocol(std::shared_ptr<cppsock::ws::ws_server_base> ws_server): m_ws_server(std::move(ws_server)) {
            m_ws_server->on_connect(this->on_ws_connect);
        }

        ~protocol();
    };
};

#endif //EVENT_H
