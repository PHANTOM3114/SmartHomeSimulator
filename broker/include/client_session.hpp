#pragma once

#include <boost/asio.hpp>
#include <memory>

#include "message_broker.hpp"

class MessageBroker;

class ClientSession : public std::enable_shared_from_this<ClientSession> {
public:
    ClientSession(boost::asio::ip::tcp::socket socket, MessageBroker& broker);

    void start();
    void stop();

    void writeMessage(const std::string& msg);

private:
    void start_read();
    void handle_read(const boost::system::error_code& error, std::size_t bytes_transferred);
    void handle_write(const boost::system::error_code& error);

    boost::asio::ip::tcp::socket m_socket;
    std::string m_read_message;
    MessageBroker& m_broker;
    boost::asio::streambuf m_buffer;
};