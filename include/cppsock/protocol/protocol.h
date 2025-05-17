//
// Created by aniket on 5/6/25.
//

#ifndef EVENT_H
#define EVENT_H
#include <boost/bimap/bimap.hpp>
#include<cppsock/ws_server_base.h>



namespace cppsock::eventing {
    struct eventing_peer {
        uint32_t peer_id;
        std::string peer_name;
        std::string peer_data;
    };
    struct event {
        uint32_t event_id;
        std::string event_name;
        std::string event_data;
    };
    std::ostream& operator<<(std::ostream& os, const cppsock::eventing::event& ev);

    class event_manager {
    public:
        std::shared_ptr<cppsock::ws::ws_server_base> m_ws_server;
        std::mutex m_mutex;
        eventing_peer m_peer;
        event_manager(std::shared_ptr<cppsock::ws::ws_server_base> ws_server,eventing_peer peer);
        std::unordered_map<std::string, std::function<void(event)>> m_callbacks;
        event_manager& on_event(const std::string& event_name, const std::function<void(event)>& callback);
        void run(uint16_t port, size_t threads);

        ~event_manager();
    };

    std::string event_serialize(event &event);
    event event_deserialize(std::string &event_string);
}

#endif //EVENT_H
