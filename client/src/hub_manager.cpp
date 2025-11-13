#include "./client/include/hub_manager.hpp"

HubManager::HubManager(const std::string& config_path)
    : m_config(config_path) // 1. Завантажуємо конфіг (або падаємо з ConfigurationError)
{
    // 2. Витягуємо дані з конфігу (або падаємо з ConfigurationError)
    m_brokerIp = m_config.getValue<std::string>("broker_ip");
    m_brokerPort = m_config.getValue<int>("broker_port");

    // 3. Створюємо "Поштаря" (BrokerConnection)
    //    Передаємо йому НАШ context і посилання на СЕБЕ
    m_broker_conn = std::make_unique<BrokerConnection>(*this, m_context);
    
    std::cout << "[HubManager] Ініціалізовано." << std::endl;
}

void HubManager::run()
{
    std::cout << "[HubManager] Запуск..." << std::endl;
    
    // 1. Кажемо "Поштарю" підключитися (це СИНХРОННИЙ виклик)
    //    Якщо він впаде, він кине ConnectionError,
    //    яке зловить наш main.cpp
    m_broker_conn->connect(m_brokerIp, m_brokerPort);

    // 2. ЗАПУСКАЄМО ГОЛОВНИЙ ЦИКЛ ОБРОБКИ
    //    Ця команда ЗАБЛОКУЄ потік (main), доки
    //    не буде викликано m_context.stop() або не станеться помилка.
    //    Всі асинхронні read_handler'и будуть працювати тут.
    std::cout << "[HubManager] Підключено. Входжу в цикл подій..." << std::endl;
    
    m_context.run(); // <--- ОСЬ ТУТ ЖИВЕ ПРОГРАМА
}

// Це наш "Секретар", якого викликає "Поштар"
void HubManager::onMessageReceived(const common::NetMessage& msg)
{
    std::cout << "[HubManager] Отримано повідомлення: " 
              << msg.serialize() << std::endl;
    
    // TODO: В майбутньому тут буде логіка:
    // 1. Десеріалізувати 'msg'
    // 2. Подивитися "топік"
    // 3. Передати повідомлення потрібному SmartLightProxy
    //    (напр. m_devices[0]->handleMessage(msg))
}