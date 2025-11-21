#include "./server/include/smart_light_emulator.hpp"
#include <fstream>
#include <iostream>
#include "common/include/smart_home_exception.hpp"

namespace asio = boost::asio;

SmartLightEmulator::SmartLightEmulator(const std::string& config_path)
    : m_config(config_path), m_topic("smarthome/living_room/light1") {
    m_broker_conn = std::make_unique<BrokerConnection>(*this, m_context);
}

void SmartLightEmulator::run() {
    std::string host = m_config.getValue<std::string>("broker_ip");
    int port = m_config.getValue<int>("broker_port");

    try {
        m_broker_conn->connect(host, port);
    } catch (const common::ConnectionError& e) {
        std::cerr << "Smart Light Emulator FATAL: " << e.what() << std::endl;
        return;
    }
    m_broker_conn->subscribe(m_topic);
    m_context.run();
}

void SmartLightEmulator::onMessageReceived(const std::string& topic, const common::NetMessage& msg) {
    if (topic != m_topic) {
        return;
    }

    save_log("Smart Light Emulator received command: " + msg.command);

    if (msg.command == common::NetMessage::CMD_TURN_ON) {
        m_power_state = true;
        save_log("Turned ON");
        publish_status();
    } else if (msg.command == common::NetMessage::CMD_TURN_OFF) {
        m_power_state = false;
        save_log("Turned OFF");
        publish_status();
    }
}

void SmartLightEmulator::publish_status() {
    common::NetMessage status_msg;
    status_msg.command = common::NetMessage::CMD_SET_STATUS;
    status_msg.payload = {{"state", m_power_state ? "on" : "off"}};

    try {
        m_broker_conn->publish(m_topic, status_msg);
    } catch (const common::ConnectionError& e) {
        std::cerr << "Smart Light Emulator Sending Error: " << e.what() << std::endl;
    }
}

void SmartLightEmulator::save_log(const std::string& entry) {
    try {
        std::ofstream log_file("emulator.log", std::ios_base::app);
        if (log_file.is_open()) {
            log_file << "[LOG] " << entry << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Smart Light Emulator Logging Error: " << e.what() << std::endl;
    }
}