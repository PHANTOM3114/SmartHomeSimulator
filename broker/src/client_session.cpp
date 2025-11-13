#include "client_session.hpp"
#include "common/include/smart_home_exception.hpp"

#include <iostream>

ClientSession::ClientSession(tcp::socket socket, MessageBroker& broker)
    : m_socket(std::move(socket)),
      m_broker(broker) {}

void ClientSession::start() {
    std::cout << "[Session] Сесія почалась. Починаю читати..." << std::endl;
    start_read();
}

void ClientSession::stop() {
    std::cout << "[Session] Сесія зупиняється." << std::endl;
    m_socket.close();
    m_broker.removeSession(shared_from_this());
}

void ClientSession::start_read() {
    asio::async_read_until(m_socket, m_buffer, "\n",
        [self = shared_from_this()](const boost::system::error_code& error, std::size_t bytes) {
            self->handle_read(error, bytes);
        });
}

void ClientSession::handle_read(const boost::system::error_code& error, std::size_t bytes_transferred) {
    
    if (!error) {
        std::istream is(&m_buffer);
        std::string line;
        std::getline(is, line);

        std::cout << "[Session] Отримано: " << line << std::endl;

        try {
            m_broker.handleMessage(line + "\n", shared_from_this());

        } catch (const smarthome::common::InvalidMessageError& e) {
            std::cerr << "[Session] Помилка повідомлення від клієнта: " << e.what() << std::endl;
            
            writeMessage("ERROR: Invalid message format\n");
        }

        start_read();
    } else {
        if (error == asio::error::eof) {
            std::cout << "[Session] Клієнт закрив з'єднання." << std::endl;
        } else {
            std::cerr << "[Session] Помилка читання: " << error.message() << std::endl;
        }
        stop();
    }
}

void ClientSession::writeMessage(const std::string& msg) {
    asio::async_write(m_socket, asio::buffer(msg),
        [self = shared_from_this()](const boost::system::error_code& error, std::size_t) {
            self->handle_write(error);
        });
}

void ClientSession::handle_write(const boost::system::error_code& error) {
    if (error) {
        std::cerr << "[Session] Помилка запису: " << error.message() << std::endl;
        stop();
    }
}