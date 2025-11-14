#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "common/include/iot_device.hpp" // <--- Твій абстрактний клас IotDevice
#include "broker_connection.hpp"       // <--- Наш "Поштар"
#include "common/include/net_message.hpp"  // <--- Наш формат повідомлень

// Попереднє оголошення, бо BrokerConnection.h не може включати це
class BrokerConnection; 

/**
 * @brief Клас-проксі для лампочки.
 * Реалізує інтерфейс IotDevice.
 * Його робота - транслювати виклики методів (turn_on)
 * у мережеві повідомлення через BrokerConnection.
 */
class SmartLightProxy : public IotDevice {
public:
    /**
     * @brief Конструктор, як на UML-діаграмі
     * @param conn Посилання на *вже існуючий* BrokerConnection
     * @param topic "Базовий топік" для цього пристрою (напр. "smarthome/living_room/light")
     */
    SmartLightProxy(BrokerConnection& conn, const std::string& topic);

    // --- Реалізація інтерфейсу IotDevice (Вимога 'c') ---

    /**
     * @brief Вмикає світло, відправляючи команду через брокера
     */
    void turn_on() override; // 'override' показує, що це віртуальна функція

    /**
     * @brief Вимикає світло, відправляючи команду через брокера
     */
    void turn_off() override;

    /**
     * @brief Повертає *локально* відомий статус пристрою
     */
    nlohmann::json get_status() override;

    // --- Додаткові публічні методи ---

    /**
     * @brief Цей метод буде викликати HubManager, коли
     * прийде повідомлення *для цього* пристрою.
     */
    void handleIncomingMessage(const common::NetMessage& msg);

private:
    // --- Поля з діаграми ---
    BrokerConnection& m_broker_conn; // Посилання на "Поштаря" (не володіємо ним)
    std::string m_topic_base;        // Напр. "smarthome/living_room/light"
    nlohmann::json m_last_known_status; // "Стан", який ми оновлюємо
};