#include <iostream>

#include "./client/include/hub_manager.hpp"
#include "./client/include/smart_light_proxy.hpp"

HubManager::HubManager(const std::string& config_path) : m_config(config_path) {
    m_brokerIp = m_config.getValue<std::string>("broker_ip");
    m_brokerPort = m_config.getValue<int>("broker_port");

    m_broker_conn = std::make_unique<BrokerConnection>(*this, m_context);

    std::string light_topic = "smarthome/living_room/light1";

    auto light_proxy = std::make_unique<SmartLightProxy>(*m_broker_conn, light_topic);

    m_devices.push_back(std::move(light_proxy));

    std::cout << "[HubManager] Initialized. " << m_devices.size() << " device(s) loaded." << std::endl;
}

void HubManager::run() {
    std::cout << "[HubManager] Starting..." << std::endl;

    m_broker_conn->connect(m_brokerIp, m_brokerPort);
    std::cout << "[HubManager] Connected." << std::endl;

    std::cout << "[HubManager] Waiting 1 second before sending trigger..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    triggerDeviceOn(0);

    std::cout << "[HubManager] Entering event loop (listening for responses)..." << std::endl;
    m_context.run();
}

void HubManager::onMessageReceived(const std::string& topic, const common::NetMessage& msg) {
    std::cout << "[HubManager] Received NetMessage. Topic: '" << topic << "'" << std::endl;

    for (const auto& device : m_devices) {
        if (device->getTopic() == topic) {
            device->handleIncomingMessage(msg);
            return;
        }
    }
}
void HubManager::triggerDeviceOn(int index) {
    if (index < 0 || index >= m_devices.size()) {
        std::cerr << "[HubManager] Error: Invalid device index: " << index << std::endl;
        return;
    }

    std::cout << "[HubManager] == TRIGGER: Turning device " << index << " ON ==" << std::endl;

    m_devices[index]->turn_on();
}