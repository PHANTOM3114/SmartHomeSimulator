#include "./broker/include/message_broker.hpp"
#include <iostream>

namespace asio = boost::asio;
using asio::ip::tcp;

const int BROKER_PORT = 9000;

MessageBroker::MessageBroker(asio::io_context& io_context)
    : m_context(io_context), m_acceptor(io_context, tcp::endpoint(tcp::v4(), BROKER_PORT)) {
    std::cout << "[Broker] Server listening on port " << BROKER_PORT << "..." << std::endl;
}

void MessageBroker::start() {
    start_accept();
}

void MessageBroker::start_accept() {
    auto new_socket = std::make_shared<tcp::socket>(m_context);

    m_acceptor.async_accept(*new_socket, [this, new_socket](const boost::system::error_code& error) {
        if (!error) {
            std::cout << "[Broker] New connection!" << std::endl;

            auto new_session = std::make_shared<ClientSession>(std::move(*new_socket), *this);

            m_sessions.push_back(new_session);

            new_session->start();
        } else {
            std::cerr << "[Broker] Accept error: " << error.message() << std::endl;
        }

        start_accept();
    });
}

void MessageBroker::handleMessage(const std::string& msg, std::shared_ptr<ClientSession> sender) {
    std::cout << "[Broker] Message received, broadcasting..." << std::endl;

    for (auto& weak_session : m_sessions) {
        if (auto session = weak_session.lock()) {

            if (session != sender) {
                session->writeMessage(msg);
            }
        }
    }
}

void MessageBroker::removeSession(std::shared_ptr<ClientSession> session_to_remove) {
    std::cout << "[Broker] Client disconnected. Removing session..." << std::endl;

    m_sessions.remove_if([&](const std::weak_ptr<ClientSession>& weak_session) {
        return weak_session.expired() || weak_session.lock() == session_to_remove;
    });
}