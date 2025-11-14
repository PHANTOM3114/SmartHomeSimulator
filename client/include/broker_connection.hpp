#pragma once

#include <boost/asio.hpp>
#include <string>
#include <thread>
#include <memory>
#include "common/include/smart_home_exception.hpp"
#include "common/include/net_message.hpp"
#include "common/include/iot_device.hpp"

class HubManager; 

class BrokerConnection {
public:
    explicit BrokerConnection(HubManager& hub, boost::asio::io_context& ctx);
    
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
    HubManager& hub;

    std::thread m_reader_thread;
    boost::asio::streambuf m_read_buffer;
};