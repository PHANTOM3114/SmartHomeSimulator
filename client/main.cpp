#include <iostream>
#include <boost/asio.hpp>

#include "./include/hub_manager.hpp"

int main()
{
try {
        // 1. Створюємо менеджер.
        //    У конструкторі він завантажить "config.json".
        //    Якщо не вийде - він кине ConfigurationError.
        HubManager manager("config.json");

        // 2. Запускаємо.
        //    Ця функція ЗАБЛОКУЄ main-потік і запустить
        //    головний цикл обробки мережевих подій (m_context.run()).
        //    Якщо не зможе підключитись - кине ConnectionError.
        manager.run();

    } catch (const common::ConfigurationError& e) {
        // Ловимо фатальну помилку конфігурації
        std::cerr << "!!! ФАТАЛЬНА ПОМИЛКА КОНФІГУРАЦІЇ !!!\n"
                  << "    Перевір config.json. Деталі: " << e.what() << std::endl;
        return 1;
    } catch (const common::ConnectionError& e) {
        // Ловимо фатальну помилку з'єднання
        std::cerr << "!!! ФАТАЛЬНА ПОМИЛКА З'ЄДНАННЯ !!!\n"
                  << "    Перевір, чи запущений Broker. Деталі: " << e.what() << std::endl;
        return 2;
    } catch (const std::exception& e) {
        // Ловимо будь-що інше (напр. std::runtime_error)
        std::cerr << "!!! НЕВІДОМА ФАТАЛЬНА ПОМИЛКА !!!\n"
                  << e.what() << std::endl;
        return 3;
    }

    // Якщо m_context.run() завершиться штатно,
    // ми дійдемо сюди.
    std::cout << "[main] HubManager завершив роботу." << std::endl;
    return 0;
}
