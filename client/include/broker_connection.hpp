#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <thread>
#include "common/include/ibroker_message_receiver.hpp"
#include "common/include/iot_device.hpp"
#include "common/include/net_message.hpp"
#include "common/include/smart_home_exception.hpp"

class BrokerConnection {
public:
    explicit BrokerConnection(IBrokerMessageReceiver& receiver, boost::asio::io_context& ctx);
    ~BrokerConnection();

    void connect(const std::string& host, int port);
    void publish(const std::string& topic, const common::NetMessage& msg);
    void subscribe(const std::string& topic);

private:
    void read_handler();
    void start_read_loop();
    void stop();
    void send_sync(const std::string& message);

    boost::asio::io_context& m_context;
    boost::asio::ip::tcp::socket m_socket;

    IBrokerMessageReceiver& m_receiver;

    std::thread m_reader_thread;
    boost::asio::streambuf m_read_buffer;
};