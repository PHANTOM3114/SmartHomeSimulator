#include <iostream>

#include <boost/asio.hpp>

#include "broker/include/message_broker.hpp"
#include "common/include/smart_home_exception.hpp"

int main() {
    try {
        boost::asio::io_context io_context;

        MessageBroker broker(io_context);
        broker.start();
        io_context.run();

    } catch (const common::SmartHomeException& e) {
        std::cerr << "Critical Smarthome error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Critical error (std): " << e.what() << std::endl;
        return 1;
    }

    return 0;
}