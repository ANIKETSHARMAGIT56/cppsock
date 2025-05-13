//
// Created by aniket on 4/29/25.
//

#include "cppsock/ws_impl/boost_server.h"


#include <boost/asio/as_tuple.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include <memory>
#include <unordered_map>
#include <vector>
#include <thread>
#include <iostream>
#include <boost/bimap/bimap.hpp>

namespace cppsock::ws {
    namespace beast = boost::beast;
    namespace http = beast::http;
    namespace websocket = beast::websocket;
    namespace net = boost::asio;

    using boost_socket_t = websocket::stream<beast::tcp_stream>;

    // Define the private `client_manager` class
    class ws_server_boost_impl::client_manager {
    private:
        // Map client IDs to some metadata, e.g., their WebSocket state
        boost::bimaps::bimap<size_t, std::shared_ptr<boost_socket_t> > m_clients;
        size_t next_client_id = 0;
        std::mutex m_mutex;


    public:
        client_manager() = default;

        // Placeholder for managing active WebSocket sessions
        // Add a new client and return its unique client ID
        size_t add_client(boost_socket_t socket) {
            next_client_id++;
            std::lock_guard<std::mutex> lock(m_mutex);
            m_clients.insert(
                boost::bimaps::bimap<size_t, std::shared_ptr<boost_socket_t> >::value_type(
                    next_client_id - 1, std::make_shared<boost_socket_t>(std::move(socket))));
            return next_client_id - 1;
        }

        // Remove a client by its ID
        void remove_client(const size_t client_id) {
            auto m_ws = m_clients.left.at(client_id);
            if (m_ws->is_open()) {
                m_ws->async_close(
                    boost::beast::websocket::close_reason{
                        static_cast<boost::beast::websocket::close_code>(1001), "manual closure"
                    },
                    [&](boost::beast::error_code ec) {
                        std::lock_guard<std::mutex> lock(m_mutex);
                        m_clients.left.erase(client_id);
                    }
                );
            }
        }

        // Check if the client is still connected
        bool is_connected(size_t client_id) {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_clients.left.find(client_id) != m_clients.left.end();
        }

        bool send(size_t ws_client_id, const cppsock_str &message) {
            try {
                // Access the socket associated with the client ID using `left.at`
                auto &socket = *m_clients.left.at(ws_client_id);
                boost::system::error_code ec;

                // Asynchronously send the message using the WebSocket
                // socket.async_write(
                //     boost::asio::buffer(message), // Message buffer
                //     [&](const boost::system::error_code &ec_local, std::size_t bytes_written) {
                //         if (ec_local) {
                //             ec = ec_local;
                //         }
                //     send_mutex.unlock();
                //     }
                //     );
                socket.text(true);
                socket.write(boost::asio::buffer(message), ec);
                if (ec) {
                    return false;
                }
                return true;
            } catch (const std::out_of_range &) {
                std::cerr << "Client ID " << ws_client_id << " not found." << std::endl;
                return false;
            }
        }

        std::weak_ptr<boost_socket_t> get_weak_ptr(size_t client_id) {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto it = m_clients.left.find(client_id); // Search by client_id
            if (it != m_clients.left.end()) {
                return it->second; // Convert the shared_ptr to weak_ptr and return
            }
            return {}; // Return an empty weak_ptr if client_id is not found
        }
    };

    // Constructor and Destructor
    ws_server_boost_impl::ws_server_boost_impl() : m_client_manager(std::make_unique<client_manager>()) {
    }

    ws_server_boost_impl::~ws_server_boost_impl() = default;

    void ws_server_boost_impl::run(uint16_t port, size_t threads) {
        net::io_context m_ioc;

    auto address = net::ip::make_address("0.0.0.0");

    net::co_spawn(
        m_ioc,
        [this, address, port, &m_ioc]() -> net::awaitable<void> {
            const auto executor = co_await net::this_coro::executor;
            net::ip::tcp::acceptor acceptor(executor, {address, port});

            for (;;) {
                auto [ec, socket] = co_await acceptor.async_accept(net::as_tuple(net::use_awaitable));
                if (ec) {
                    std::cerr << "Accept error: " << ec.message() << "\n";
                    continue;
                }

                net::co_spawn(
                    executor,
                    [this, sock = std::move(socket), &m_ioc]() mutable -> net::awaitable<void> {
                        try {
                            websocket::stream<beast::tcp_stream> ws(std::move(sock));
                            auto [ec_accept] = co_await ws.async_accept(net::as_tuple(net::use_awaitable));
                            if (ec_accept) {
                                std::cerr << "Accept error: " << ec_accept.message() << "\n";
                                co_return;
                            }

                            const size_t client_id = m_client_manager->add_client(std::move(ws));
                            m_connect_callback(client_id);

                            auto weak_stream = m_client_manager->get_weak_ptr(client_id);

                            for (;;) {
                                beast::flat_buffer buffer;
                                auto locked = weak_stream.lock();
                                if (!locked) {
                                    m_disconnect_callback(client_id, UNKNOWN);
                                    m_client_manager->remove_client(client_id);
                                    co_return;
                                }

                                auto [ec_read, _] = co_await locked->async_read(buffer, net::as_tuple);
                                if (ec_read == websocket::error::closed) {
                                    m_disconnect_callback(client_id, UNKNOWN);
                                    m_client_manager->remove_client(client_id);
                                    co_return;
                                }
                                if (ec_read) {
                                    m_error_callback(client_id);
                                    m_client_manager->remove_client(client_id);
                                    co_return;
                                }

                                std::string message = beast::buffers_to_string(buffer.data());
                                net::post(m_ioc, [this, client_id, message = std::move(message)] {
                                    try {
                                        m_message_callback(client_id, message);
                                    } catch (const std::exception& e) {
                                        std::cerr << "Callback exception: " << e.what() << '\n';
                                    }
                                });
                            }
                        } catch (const std::exception& e) {
                            std::cerr << "Session error: " << e.what() << '\n';
                        }
                    },
                    net::detached);
            }
        },
        net::detached);

    // Launch I/O threads
    std::vector<std::thread> v;
    for (size_t i = 1; i < threads; ++i) {
        v.emplace_back([&] { m_ioc.run(); });
    }
    m_ioc.run();  // main thread
}

    bool ws_server_boost_impl::send(size_t client_id, const cppsock_str &message) {
        return m_client_manager->send(client_id, message);
    }

    bool ws_server_boost_impl::close(size_t client_id, uint16_t code, const cppsock_str &reason) {
        m_client_manager->remove_client(client_id);
        return 1;
    }

    bool ws_server_boost_impl::is_connected(size_t client_id) {
        return m_client_manager->is_connected(client_id);
    }
} // namespace cppsock::ws





