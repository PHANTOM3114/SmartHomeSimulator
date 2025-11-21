#pragma once

#include <list>
#include <memory>

#include <boost/asio.hpp>

#include "client_session.hpp"

class ClientSession;

class MessageBroker {
public:
    MessageBroker(boost::asio::io_context& io_context);

    void start();
    void handleMessage(const std::string& msg, std::shared_ptr<ClientSession> sender);
    void removeSession(std::shared_ptr<ClientSession> session);

private:
    void start_accept();

    void handle_accept(std::shared_ptr<ClientSession> new_session, const boost::system::error_code& error);

    boost::asio::io_context& m_context;
    boost::asio::ip::tcp::acceptor m_acceptor;

    std::list<std::weak_ptr<ClientSession>> m_sessions;
};