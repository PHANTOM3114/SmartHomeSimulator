#include <iostream>
#include <boost/asio.hpp>

#include "./include/hub_manager.hpp"

int main()
{
    try {
        HubManager manager("config.json");

        std::thread manager_thread([&manager]() {
            try {
                manager.run(); // Цей потік тепер "завис" в m_context.run()
            } catch (const std::exception& e) {
                std::cerr << "!!! Помилка в потоці менеджера: " << e.what() << std::endl;
            }
        });

        std::cout << "[main] Чекаю 2 секунди на підключення..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));

        manager.triggerDeviceOn(0);

        std::cout << "[main] Команду відправлено. Чекаю 5 секунд..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));

        std::cout << "[main] Завершую роботу..." << std::endl;

        exit(0);


    } catch (const common::ConfigurationError& e) {
        std::cerr << "!!! ФАТАЛЬНА ПОМИЛКА КОНФІГУРАЦІЇ: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "!!! НЕВІДОМА ФАТАЛЬНА ПОМИЛКА: " << e.what() << std::endl;
        return 3;
    }

    return 0;
}
