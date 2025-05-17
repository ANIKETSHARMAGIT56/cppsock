//
// Created by aniket on 5/7/25.
//
#include "cppsock/protocol/protocol.h"

#include <boost/asio/buffer.hpp>
#include <glaze/glaze.hpp>
#include <utility>

namespace cppsock::eventing {

    std::ostream& operator<<(std::ostream& os, const cppsock::eventing::event& ev) {
        os << ev.event_name << " --> " << ev.event_data;
        return os;
    }
    event_manager::~event_manager() {}


    std::string event_serialize(event &event) {
        std::string buffer;
        auto ec = glz::write_json(event,buffer);
        if (ec) {
            throw std::runtime_error("error serializing event");
        }
        return buffer;
    }
    event event_deserialize(const std::string& event_string) {
        event deserialized_event;
        auto ec = glz::read_json(deserialized_event,event_string);
        if (ec) {
            throw std::runtime_error("error deserializing event");
        }
        return deserialized_event;
    }





    event_manager::event_manager(std::shared_ptr<cppsock::ws::ws_server_base>ws_server, eventing_peer peer):m_ws_server(std::move(ws_server)),m_peer(peer) {
        // m_ws_server->on_connect(...)
        m_ws_server->on_message([&](size_t client_id, const std::string &message) {
            try {
                event ev = event_deserialize(message);
                m_callbacks[ev.event_name](ev);
            }
            catch (...){
            }
                return 0;

        }).on_connect([&](size_t client_id){});
    }
    event_manager& event_manager::on_event(const std::string& event_name, const std::function<void(event)>& callback) {
        m_callbacks[event_name] = callback;
        return *this;
    }
    void event_manager::run(uint16_t port, size_t threads) {
        m_ws_server->run(port,threads);
    }

}
