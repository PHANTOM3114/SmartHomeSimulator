#pragma once

#include <boost/asio.hpp>
#include <memory>

#include "./client/include/broker_connection.hpp"
#include "common/include/json_load_config.hpp"
#include "common/include/net_message.hpp"

class SmartLightEmulator : public IBrokerMessageReceiver {
public:
    explicit SmartLightEmulator(const std::string& config_path);
    void run();
    void save_log(const std::string& entry);

    void onMessageReceived(const std::string& topic, const common::NetMessage& msg) override;  // <--- ЗМІНА

private:
    void publish_status();

    bool m_power_state = false;
    std::string m_topic;

    boost::asio::io_context m_context;
    common::JsonLoadConfig m_config;
    std::unique_ptr<BrokerConnection> m_broker_conn;
};