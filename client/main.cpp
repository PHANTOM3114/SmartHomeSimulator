#include <iostream>
#include <boost/asio.hpp>

#include "./include/hub_manager.hpp"

int main()
{
    try {
        HubManager manager("config.json");

        // --- ТЕСТОВИЙ БЛОК ---
        // Ми не можемо викликати 'run' одразу, бо він блокуючий.
        // Тому ми запускаємо 'run' в окремому потоці,
        // а в main-потоці надсилаємо команди.

        // 1. Запускаємо "Офіс" (HubManager) в фоновому потоці
        std::thread manager_thread([&manager]() {
            try {
                manager.run(); // Цей потік тепер "завис" в m_context.run()
            } catch (const std::exception& e) {
                std::cerr << "!!! Помилка в потоці менеджера: " << e.what() << std::endl;
            }
        });

        // Даємо менеджеру час на підключення
        std::cout << "[main] Чекаю 2 секунди на підключення..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));

        // 2. --- ОСЬ ТВІЙ ЛАНЦЮГ ---
        //    Кажемо "Офісу" натиснути кнопку "ввімкнути" для девайсу 0
        manager.triggerDeviceOn(0);

        // 3. Чекаємо, щоб побачити відповідь (якщо вона прийде)
        std::cout << "[main] Команду відправлено. Чекаю 5 секунд..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));

        // 4. Завершуємо
        std::cout << "[main] Завершую роботу..." << std::endl;
        // (Деструктор HubManager викличе stop() і все зупинить)
        
        // (В реальному житті ми б тут зупиняли m_context.stop() 
        //  і робили manager_thread.join(), але для тесту це складно)
        
        // Для простоти, ми просто вбиваємо програму.
        // Деструктори зроблять join() потоків при виході.
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
