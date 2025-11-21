#include <boost/asio.hpp>
#include <iostream>

#include "./include/hub_manager.hpp"

int main() {
    try {
        HubManager manager("config.json");

        manager.run();

    } catch (const common::ConfigurationError& e) {
        std::cerr << "!!! CLIENT: FATAL CONFIGURATION ERROR: " << e.what() << std::endl;
        return 1;
    } catch (const common::ConnectionError& e) {
        std::cerr << "!!! CLIENT: FATAL CONNECTION ERROR: " << e.what() << std::endl;
        return 2;
    } catch (const std::exception& e) {
        std::cerr << "!!! CLIENT: UNKNOWN FATAL ERROR: " << e.what() << std::endl;
        return 3;
    }

    std::cout << "[Client] Exiting." << std::endl;

    return 0;
}
