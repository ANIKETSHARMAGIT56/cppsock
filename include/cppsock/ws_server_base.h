//
// Created by aniket on 4/28/25.
//

#ifndef WS_SERVER_BASE_H
#define WS_SERVER_BASE_H


#include "types.h"

#include <memory>

namespace cppsock{
namespace ws{
class ws_server_base {
public:
    ws_server_base() = default;
    virtual ~ws_server_base() = default;

    ws_server_base& on_connect(const connect_callback_t& connect_callback) {
        m_connect_callback = connect_callback;
        return *this;
    }

    ws_server_base& on_message(const message_callback_t& message_callback) {
        m_message_callback = message_callback;
        return *this;
    }

    ws_server_base& on_disconnect(const disconnect_callback_t& disconnect_callback) {
        m_disconnect_callback = disconnect_callback;
        return *this;
    }

    ws_server_base& on_error(const error_callback_t& error_callback) {
        m_error_callback = error_callback;
        return *this;
    }
    virtual bool send(size_t client_id, const cppsock_str& message) = 0;
    virtual bool close(size_t client_id, uint16_t code, const cppsock_str& reason) = 0;
    virtual bool is_connected(size_t client_id) = 0;
    virtual void run(uint16_t port, size_t threads) = 0;

private:
    class client_manager; // <- forward declare
protected:
    connect_callback_t    m_connect_callback = [](size_t){};
    message_callback_t    m_message_callback = [](size_t,std::string){};
    disconnect_callback_t m_disconnect_callback = [](size_t,DISCONNECT_REASON){};
    error_callback_t      m_error_callback = [](size_t){};
};

}
}


#endif //WS_SERVER_BASE_H
