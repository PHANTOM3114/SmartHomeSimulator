#include <string>
#include <iostream>
#include <vector>

#include "common/include/iot_device.hpp"
#include "common/include/json_load_config.hpp"
#include "common/include/net_message.hpp"

#include "broker_connection.hpp"

namespace common {
    struct NetMessage;
}

class HubManager {
public:
    /**
     * @brief Конструктор. Завантажує конфіг, але НЕ підключається.
     */
    explicit HubManager(const std::string& config_path);

    /**
     * @brief Запускає логіку: підключається і блокує потік (входить в io_context.run())
     */
    void run();

    /**
     * @brief Колбек. Викликається з BrokerConnection, коли приходить повідомлення.
     */
    void onMessageReceived(const common::NetMessage& msg);

private:
    // --- ОСНОВНІ КОМПОНЕНТИ ---
    boost::asio::io_context m_context; // <--- 1. ЄДИНИЙ io_context на весь клієнт
    common::JsonLoadConfig m_config;

    // --- З'ЄДНАННЯ ---
    // unique_ptr, бо ми створюємо його в *тілі* конструктора
    std::unique_ptr<BrokerConnection> m_broker_conn; 

    // --- ДАНІ З КОНФІГУ ---
    std::string m_brokerIp;
    int m_brokerPort;

    // TODO: Тут буде список девайсів
    // std::vector<std::unique_ptr<IotDevice>> m_devices;
};
    