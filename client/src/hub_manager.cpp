#include "./client/include/hub_manager.hpp"
#include "./client/include/smart_light_proxy.hpp"

HubManager::HubManager(const std::string& config_path)
    : m_config(config_path) 
{
    m_brokerIp = m_config.getValue<std::string>("broker_ip");
    m_brokerPort = m_config.getValue<int>("broker_port");

    m_broker_conn = std::make_unique<BrokerConnection>(*this, m_context);
    
    std::string light_topic = "smarthome/living_room/light1";
    
    auto light_proxy = std::make_unique<SmartLightProxy>(*m_broker_conn, light_topic);

    m_devices.push_back(std::move(light_proxy));
    
    std::cout << "[HubManager] Ініціалізовано. " << m_devices.size() << " девайс(ів) завантажено." << std::endl;
}

void HubManager::run()
{
    std::cout << "[HubManager] Запуск..." << std::endl;
    m_broker_conn->connect(m_brokerIp, m_brokerPort);
    std::cout << "[HubManager] Підключено. Входжу в цикл подій..." << std::endl;
    m_context.run();
}

void HubManager::onMessageReceived(const common::NetMessage& msg)
{
    std::cout << "[HubManager] Отримано повідомлення: " << msg.serialize() << std::endl;
}

void HubManager::triggerDeviceOn(int index)
{
    if (index < 0 || index >= m_devices.size()) {
        std::cerr << "[HubManager] Помилка: Невірний індекс девайсу: " << index << std::endl;
        return;
    }

    std::cout << "[HubManager] == ТРИГЕР: Вмикаю девайс " << index << " ==" << std::endl;
    
    m_devices[index]->turn_on();
}