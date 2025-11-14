#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "common/include/iot_device.hpp"
#include "broker_connection.hpp"
#include "common/include/net_message.hpp"

class BrokerConnection; 

class SmartLightProxy : public IotDevice {
public:
    SmartLightProxy(BrokerConnection& conn, const std::string& topic);

    void turn_on() override;

    void turn_off() override;

    std::string getTopic() const;

    nlohmann::json get_status() override;

    void handleIncomingMessage(const common::NetMessage& msg);

private:
    BrokerConnection& m_broker_conn;
    std::string m_topic_base;
    nlohmann::json m_last_known_status;
};