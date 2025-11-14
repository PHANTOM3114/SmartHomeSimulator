#include "./client/include/hub_manager.hpp"
#include "./client/include/broker_connection.hpp" 
#include <iostream>
#include <nlohmann/json.hpp>

namespace asio = boost::asio;
using asio::ip::tcp;

BrokerConnection::BrokerConnection(HubManager& hub_manager, asio::io_context& ctx)
    : m_context(ctx),
      m_socket(ctx),
      hub(hub_manager)
{
}

BrokerConnection::~BrokerConnection()
{
    stop();
}

void BrokerConnection::connect(const std::string& host, int port)
{
    try {
        tcp::resolver resolver(m_context);
        
        auto endpoints = resolver.resolve(host, std::to_string(port));

        boost::system::error_code ec;
        asio::connect(m_socket, endpoints, ec);

        if (ec) {
            throw common::ConnectionError("BrokerConnection: Не вдалося підключитися: " + ec.message());
        }

        std::cout << "[BrokerConnection] Успішно підключено до " << host << ":" << port << std::endl;

        start_read_loop();

    } catch (const std::exception& e) {
        throw common::ConnectionError("BrokerConnection: Помилка підключення: " + std::string(e.what()));
    }
}

void BrokerConnection::publish(const std::string& topic, const common::NetMessage& msg)
{
    nlohmann::json j;
    j["action"] = "publish";
    j["topic"] = topic;
    
    j["payload"] = msg.serialize(); 

    send_sync(j.dump() + "\n");
}

void BrokerConnection::subscribe(const std::string& topic)
{
    nlohmann::json j;
    j["action"] = "subscribe";
    j["topic"] = topic;

    send_sync(j.dump() + "\n");
    
    std::cout << "[BrokerConnection] Відправлено 'фейковий' запит на підписку: " << topic << std::endl;
}

void BrokerConnection::stop()
{
    try {
        if (!m_context.stopped()) {
            m_context.stop();
        }
        
        if (m_reader_thread.joinable()) {
            m_reader_thread.join();
        }

        if (m_socket.is_open()) {
            m_socket.close();
        }
    } catch (const std::exception& e) {
        std::cerr << "[BrokerConnection] Помилка під час stop(): " << e.what() << std::endl;
    }
}

void BrokerConnection::start_read_loop()
{
    read_handler();

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
    asio::async_read_until(m_socket, m_read_buffer, "\n",
        [this](const boost::system::error_code& ec, std::size_t bytes_transferred)
        {
            if (!ec) {
                std::istream is(&m_read_buffer);
                std::string line;
                std::getline(is, line);

                if (line.empty()) {
                    read_handler();
                    return;
                }

                try {
                    common::NetMessage msg = common::NetMessage::deserialize(line);

                    hub.onMessageReceived(msg);

                } catch (const common::InvalidMessageError& e) {
                    std::cerr << "[BrokerConnection] Отримано 'бите' повідомлення: " << e.what() << std::endl;
                }

                read_handler();
                
            } else {
                std::cerr << "[BrokerConnection] Помилка читання (з'єднання розірвано): " << ec.message() << std::endl;
            }
        });
}

void BrokerConnection::send_sync(const std::string& message)
{
    boost::system::error_code ec;
    
    asio::write(m_socket, asio::buffer(message), ec);

    if (ec) {
        throw common::ConnectionError("BrokerConnection: Помилка відправки: " + ec.message());
    }
}