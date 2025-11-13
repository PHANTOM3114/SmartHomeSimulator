#include "./client/include/hub_manager.hpp"
#include "./client/include/broker_connection.hpp" 
#include <iostream>
#include <nlohmann/json.hpp>

namespace asio = boost::asio;
using asio::ip::tcp;

// --- 1. Конструктор та Деструктор ---

BrokerConnection::BrokerConnection(HubManager& hub_manager, asio::io_context& ctx)
    : m_context(ctx),         // Ініціалізуємо посилання на *спільний* context
      m_socket(ctx),          // Ініціалізуємо сокет *цим* context'ом
      hub(hub_manager)        // Ініціалізуємо посилання на менеджера
{
    // Конструктор порожній, вся магія в списку ініціалізації
}

BrokerConnection::~BrokerConnection()
{
    // Деструктор просто викликає stop(),
    // щоб коректно зупинити потік та закрити сокет
    stop();
}

// --- 2. Публічні методи (Інтерфейс) ---

void BrokerConnection::connect(const std::string& host, int port)
{
    try {
        tcp::resolver resolver(m_context);
        
        // 1. Визначаємо, куди підключатися
        auto endpoints = resolver.resolve(host, std::to_string(port));

        // 2. Робимо СИНХРОННУ (блокуючу) спробу підключення
        boost::system::error_code ec;
        asio::connect(m_socket, endpoints, ec);

        if (ec) {
            // Якщо не вийшло - кидаємо наш кастомний виняток
            throw common::ConnectionError("BrokerConnection: Не вдалося підключитися: " + ec.message());
        }

        std::cout << "[BrokerConnection] Успішно підключено до " << host << ":" << port << std::endl;

        // 3. Якщо все ОК - запускаємо АСИНХРОННИЙ цикл читання в окремому потоці
        start_read_loop();

    } catch (const std::exception& e) {
        // Перехоплюємо будь-яку помилку (в т.ч. resolve) і кидаємо наш тип
        throw common::ConnectionError("BrokerConnection: Помилка підключення: " + std::string(e.what()));
    }
}

void BrokerConnection::publish(const std::string& topic, const common::NetMessage& msg)
{
    // Формуємо "обгортку" для нашого тупого брокера.
    // Всі отримають цей JSON, а далі самі розберуться.
    nlohmann::json j;
    j["action"] = "publish";
    j["topic"] = topic;
    
    // Припускаємо, що NetMessage::serialize() повертає рядок
    // (або інший JSON, nlohmann це "з'їсть")
    j["payload"] = msg.serialize(); 

    // Відправляємо синхронно
    send_sync(j.dump() + "\n");
}

void BrokerConnection::subscribe(const std::string& topic)
{
    // Це "фейкова" підписка. Ми просто відправляємо повідомлення,
    // яке брокер розішле всім. Це просто для логіки,
    // реальної фільтрації на сервері немає.
    
    nlohmann::json j;
    j["action"] = "subscribe";
    j["topic"] = topic;

    send_sync(j.dump() + "\n");
    
    std::cout << "[BrokerConnection] Відправлено 'фейковий' запит на підписку: " << topic << std::endl;
}


// --- 3. Приватні методи (Внутрішня логіка) ---

void BrokerConnection::stop()
{
    // Коректна зупинка
    try {
        // 1. Повідомляємо asio, що робота завершена (це розблокує m_context.run())
        if (!m_context.stopped()) {
            m_context.stop();
        }
        
        // 2. Чекаємо, поки потік m_reader_thread коректно завершиться
        if (m_reader_thread.joinable()) {
            m_reader_thread.join();
        }

        // 3. Закриваємо сокет
        if (m_socket.is_open()) {
            m_socket.close();
        }
    } catch (const std::exception& e) {
        std::cerr << "[BrokerConnection] Помилка під час stop(): " << e.what() << std::endl;
    }
}

void BrokerConnection::start_read_loop()
{
    // 1. "Праймінг" - запускаємо першу асинхронну операцію читання
    read_handler();

    // 2. Запускаємо m_context.run() в окремому потоці.
    //    Цей потік тепер буде обробляти *всі* асинхронні колбеки.
    m_reader_thread = std::thread([this]() {
        try {
            m_context.run();
            std::cout << "[BrokerConnection] Потік читання завершив роботу." << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[BrokerConnection] Потік читання впав: " << e.what() << std::endl;
        }
    });
}

void BrokerConnection::read_handler()
{
    // Асинхронно читаємо з сокета, доки не зустрінемо "\n"
    asio::async_read_until(m_socket, m_read_buffer, "\n",
        [this](const boost::system::error_code& ec, std::size_t bytes_transferred)
        {
            // Ця лямбда - наш колбек. Вона спрацює, коли дані прийдуть.
            
            if (!ec) {
                // 1. Якщо все добре - витягуємо дані з буфера
                std::istream is(&m_read_buffer);
                std::string line;
                std::getline(is, line); // line зараз містить дані

                if (line.empty()) {
                    // Просто ігноруємо порожні рядки
                    read_handler(); // <--- Рекурсивний цикл
                    return;
                }

                try {
                    // 2. Десеріалізуємо (це має бути у NetMessage)
                    //    Припускаємо, що deserialize статичний
                    common::NetMessage msg = common::NetMessage::deserialize(line);

                    // 3. ПОВІДОМЛЯЄМО МЕНЕДЖЕРА
                    //    !!! HubManager має реалізувати цей метод! !!!
                    hub.onMessageReceived(msg);

                } catch (const common::InvalidMessageError& e) {
                    std::cerr << "[BrokerConnection] Отримано 'бите' повідомлення: " << e.what() << std::endl;
                }

                // 4. Запускаємо наступну операцію читання (це і є наш "цикл")
                read_handler();
                
            } else {
                // Помилка (напр. сервер від'єднався, asio::error::eof)
                std::cerr << "[BrokerConnection] Помилка читання (з'єднання розірвано): " << ec.message() << std::endl;
                // Ми НЕ викликаємо read_handler() знову,
                // таким чином цикл читання завершується.
                // Потік (m_context.run()) зупиниться сам.
            }
        });
}


/**
 * @brief Приватна хелпер-функція для СИНХРОННОЇ відправки
 */
void BrokerConnection::send_sync(const std::string& message)
{
    boost::system::error_code ec;
    
    // Синхронно відправляємо (блокуємо потік, доки не відправиться)
    asio::write(m_socket, asio::buffer(message), ec);

    if (ec) {
        // Якщо не вдалося - кидаємо виняток
        throw common::ConnectionError("BrokerConnection: Помилка відправки: " + ec.message());
    }
}