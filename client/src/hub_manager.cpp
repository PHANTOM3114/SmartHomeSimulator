#include "./client/include/hub_manager.hpp"
#include "./client/include/smart_light_proxy.hpp"

HubManager::HubManager(const std::string& config_path)
    : m_config(config_path) 
{
    // 1. Завантажуємо конфіг
    m_brokerIp = m_config.getValue<std::string>("broker_ip");
    m_brokerPort = m_config.getValue<int>("broker_port");

    // 2. Створюємо "Поштаря" (BrokerConnection)
    m_broker_conn = std::make_unique<BrokerConnection>(*this, m_context);
    
    // 3. СТВОРЮЄМО НАШІ ДЕВАЙСИ (Проксі)
    //    Ми передаємо "Поштарю" посилання на *нашого* m_broker_conn
    //    Це і є той "ланцюг", який ти хотів.
    
    // (Для тесту захардкодимо топік)
    std::string light_topic = "smarthome/living_room/light1";
    
    // Створюємо проксі
    auto light_proxy = std::make_unique<SmartLightProxy>(*m_broker_conn, light_topic);

    // Додаємо його у вектор.
    // light_proxy (типу SmartLightProxy*) автоматично
    // кастується до IotDevice* (поліморфізм!)
    m_devices.push_back(std::move(light_proxy));

    // TODO: Можна додати ще (інший топік, інший проксі)
    // m_devices.push_back(std::make_unique<SmartThermostatProxy>(...));
    
    std::cout << "[HubManager] Ініціалізовано. " << m_devices.size() << " девайс(ів) завантажено." << std::endl;
}

void HubManager::run()
{
    std::cout << "[HubManager] Запуск..." << std::endl;
    m_broker_conn->connect(m_brokerIp, m_brokerPort);
    std::cout << "[HubManager] Підключено. Входжу в цикл подій..." << std::endl;
    m_context.run();
}

// Це наш "Секретар" (заглушка)
void HubManager::onMessageReceived(const common::NetMessage& msg)
{
    std::cout << "[HubManager] Отримано повідомлення: " << msg.serialize() << std::endl;
    
    // ☢️ ТЕХ. БОРГ: В майбутньому цей метод має парсити 'msg',
    // знаходити потрібний 'm_devices[i]' і викликати 
    // m_devices[i]->handleIncomingMessage(msg)
}

// --- РЕАЛІЗАЦІЯ МЕТОДУ ДЛЯ MAIN ---
void HubManager::triggerDeviceOn(int index)
{
    // Перевірка, щоб не впасти
    if (index < 0 || index >= m_devices.size()) {
        std::cerr << "[HubManager] Помилка: Невірний індекс девайсу: " << index << std::endl;
        return;
    }

    std::cout << "[HubManager] == ТРИГЕР: Вмикаю девайс " << index << " ==" << std::endl;
    
    // --- ОСЬ ТВОЯ ЛОГІКА "ПО ІНДЕКСУ" ---
    // 1. Звертаємось по індексу (отримуємо IotDevice*)
    // 2. Викликаємо віртуальний метод turn_on() (це пізнє зв'язування, вимога 'b')
    m_devices[index]->turn_on();
    
    // 3. 'SmartLightProxy::turn_on()' (Фасад) 
    //    викликає 'm_broker_conn.publish()'
}