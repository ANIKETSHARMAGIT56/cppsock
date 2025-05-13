//
// Created by aniket on 4/29/25.
//

#ifndef WS_SERVER_BOOST_IMPL_H
#define WS_SERVER_BOOST_IMPL_H

#include "cppsock/ws_server_base.h"

namespace cppsock::ws {

    class ws_server_boost_impl final : public ws_server_base {
    public:
        ws_server_boost_impl();
        ~ws_server_boost_impl() override;

        // The run function will start the WebSocket server
        bool send(size_t ws_client_id, const cppsock_str& message) override;
        bool close(size_t ws_client_id, uint16_t code, const cppsock_str& reason) override;
        bool is_connected(size_t ws_client_id) override;
        void run(uint16_t port, size_t threads) override;

    private:
        class client_manager; // Forward declaration for PImpl
        std::unique_ptr<client_manager> m_client_manager; // PImpl member
    };

} // namespace cppsock::ws

#endif // WS_SERVER_BOOST_IMPL_H