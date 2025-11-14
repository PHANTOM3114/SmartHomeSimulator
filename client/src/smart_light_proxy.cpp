#include "./client/include/smart_light_proxy.hpp"
#include "./client/include/broker_connection.hpp"       // Потрібен для m_broker_conn.publish
#include "common/include/net_message.hpp"      // Потрібен для створення повідомлень

#include <iostream> // Для логування

/**
 * @brief Конструктор
 * Ініціалізує посилання на брокера, топік та початковий стан.
 */
SmartLightProxy::SmartLightProxy(BrokerConnection& conn, const std::string& topic)
    : m_broker_conn(conn),
      m_topic_base(topic)
{
    // Встановлюємо початковий "відомий" стан (напр. "вимкнено")
    m_last_known_status = {
        {"state", "off"},
        {"brightness", 0}
    };
    std::cout << "[SmartLightProxy] Створено для топіку: " << m_topic_base << std::endl;
}

/**
 * @brief Відправляє команду "увімкнути"
 */
void SmartLightProxy::turn_on()
{
    std::cout << "[SmartLightProxy] Відправляю команду 'turn_on' на топік: " << m_topic_base << std::endl;
    
    // 1. Створюємо повідомлення (згідно діаграми NetMessage)
    common::NetMessage msg;
    msg.command = common::NetMessage::CMD_TURN_ON; // "cmd:turn_on"
    msg.payload = {}; // Команда "увімкнути" не потребує тіла
    
    // 2. Віддаємо "Поштарю" (BrokerConnection)
    try {
        m_broker_conn.publish(m_topic_base, msg);
    } catch (const common::ConnectionError& e) {
        std::cerr << "[SmartLightProxy] Помилка відправки 'turn_on': " << e.what() << std::endl;
        // Тут можна оновити m_last_known_status, щоб показати "error"
    }
}

/**
 * @brief Відправляє команду "вимкнути"
 */
void SmartLightProxy::turn_off()
{
    std::cout << "[SmartLightProxy] Відправляю команду 'turn_off' на топік: " << m_topic_base << std::endl;

    // 1. Створюємо повідомлення
    common::NetMessage msg;
    msg.command = common::NetMessage::CMD_TURN_OFF; // "cmd:turn_off"
    msg.payload = {};
    
    // 2. Віддаємо "Поштарю"
    try {
        m_broker_conn.publish(m_topic_base, msg);
    } catch (const common::ConnectionError& e) {
        std::cerr << "[SmartLightProxy] Помилка відправки 'turn_off': " << e.what() << std::endl;
    }
}

/**
 * @brief Повертає ОСТАННІЙ ВІДОМИЙ СТАН (з локальної змінної)
 * Цей метод НЕ робить мережевий запит.
 */
nlohmann::json SmartLightProxy::get_status()
{
    std::cout << "[SmartLightProxy] Надаю локальний статус: " << m_last_known_status.dump() << std::endl;
    return m_last_known_status;
}

/**
 * @brief Обробник вхідних повідомлень (його викликає HubManager)
 */
void SmartLightProxy::handleIncomingMessage(const common::NetMessage& msg)
{
    std::cout << "[SmartLightProxy] Отримав вхідне повідомлення: " << msg.command << std::endl;

    // Ми очікуємо, що емулятор (SmartLightEmulator)
    // відповість нам командою "cmd:set_status"
    if (msg.command == common::NetMessage::CMD_SET_STATUS) 
    {
        // Якщо це команда оновлення статусу - оновлюємо свій локальний стан
        m_last_known_status = msg.payload;
        
        std::cout << "[SmartLightProxy] Статус оновлено: " 
                  << m_last_known_status.dump() << std::endl;
        
        // TODO: Тут можна було б "викинути" сигнал (Qt signal)
        // щоб GUI (QML) оновився, якщо б ми його тут мали.
    }
    // (інші команди, якщо потрібно, можна обробити тут)
}