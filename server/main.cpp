#include <iostream>
#include "smart_light_emulator.hpp"

int main() {
    try {
        SmartLightEmulator emulator("config.json");

        emulator.run();

    } catch (const common::ConfigurationError& e) {
        std::cerr << "Smart Light Emulator: FATAL CONFIGURATION ERROR: " << e.what() << std::endl;
        return 1;
    } catch (const common::ConnectionError& e) {
        std::cerr << "Smart Light Emulator: FATAL CONNECTION ERROR: " << e.what() << std::endl;
        return 2;
    } catch (const std::exception& e) {
        std::cerr << "Smart Light Emulator: UNKNOWN FATAL ERROR: " << e.what() << std::endl;
        return 3;
    }

    std::cout << "Smart Light Emulator Work Ended." << std::endl;
    return 0;
}