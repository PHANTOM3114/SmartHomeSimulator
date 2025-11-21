#include "./client/include/smart_light_proxy.hpp"
#include "./client/include/broker_connection.hpp"
#include "common/include/net_message.hpp"

#include <iostream>

SmartLightProxy::SmartLightProxy(BrokerConnection& conn, const std::string& topic)
    : m_broker_conn(conn), m_topic_base(topic) {
    m_last_known_status = {{"state", "off"}, {"brightness", 0}};
    std::cout << "[SmartLightProxy] Created for topic: " << m_topic_base << std::endl;
}

void SmartLightProxy::turn_on() {
    std::cout << "[SmartLightProxy] Sending 'turn_on' command to topic: " << m_topic_base << std::endl;

    common::NetMessage msg;
    msg.command = common::NetMessage::CMD_TURN_ON;
    msg.payload = {};

    try {
        m_broker_conn.publish(m_topic_base, msg);
    } catch (const common::ConnectionError& e) {
        std::cerr << "[SmartLightProxy] Error sending 'turn_on': " << e.what() << std::endl;
    }
}

void SmartLightProxy::turn_off() {
    std::cout << "[SmartLightProxy] Sending 'turn_off' command to topic: " << m_topic_base << std::endl;

    common::NetMessage msg;
    msg.command = common::NetMessage::CMD_TURN_OFF;
    msg.payload = {};

    try {
        m_broker_conn.publish(m_topic_base, msg);
    } catch (const common::ConnectionError& e) {
        std::cerr << "[SmartLightProxy] Error sending 'turn_off': " << e.what() << std::endl;
    }
}

std::string SmartLightProxy::getTopic() const {
    return m_topic_base;
}

void SmartLightProxy::handleIncomingMessage(const common::NetMessage& msg) {
    std::cout << "[SmartLightProxy] (" << m_topic_base << ") Received incoming message: " << msg.command << std::endl;

    if (msg.command == common::NetMessage::CMD_SET_STATUS) {
        m_last_known_status = msg.payload;

        std::cout << "[SmartLightProxy] (" << m_topic_base << ") Status updated: " << m_last_known_status.dump()
                  << std::endl;
    }
}

nlohmann::json SmartLightProxy::get_status() {
    std::cout << "[SmartLightProxy] Providing local status: " << m_last_known_status.dump() << std::endl;
    return m_last_known_status;
}