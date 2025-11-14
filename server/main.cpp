#include "smart_light_emulator.hpp"
#include <iostream>

int main()
{
    try {
        SmartLightEmulator emulator("config.json");

        emulator.run();

    } catch (const common::ConfigurationError& e) {
        std::cerr << "!!! ЕМУЛЯТОР: ФАТАЛЬНА ПОМИЛКА КОНФІГУРАЦІЇ: " << e.what() << std::endl;
        return 1;
    } catch (const common::ConnectionError& e) {
        std::cerr << "!!! ЕМУЛЯТОР: ФАТАЛЬНА ПОМИЛКА З'ЄДНАННЯ: " << e.what() << std::endl;
        return 2;
    } catch (const std::exception& e) {
        std::cerr << "!!! ЕМУЛЯТОР: НЕВІДОМА ФАТАЛЬНА ПОМИЛКА: " << e.what() << std::endl;
        return 3;
    }

    std::cout << "[Emulator] Завершив роботу." << std::endl;
    return 0;
}