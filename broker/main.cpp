#include <iostream>

#include <boost/asio.hpp>

#include "message_broker.hpp"
#include "common/include/smart_home_exception.hpp"

int main() {
    try {
        asio::io_context io_context;

        // Тут в майбутньому буде ConfigLoader,
        // який може кинути ConfigurationError
        // Bowl::ConfigLoader config;
        // config.load("config.json"); 

        MessageBroker broker(io_context);
        broker.start();
        io_context.run();

    } 
    catch (const common::SmartHomeException& e) {
        std::cerr << "Критична помилка Smarthome: " << e.what() << std::endl;
        return 1;
    } 
    catch (const std::exception& e) {
        std::cerr << "Критична помилка (std): " << e.what() << std::endl;
        return 1;
    }

    return 0;
}