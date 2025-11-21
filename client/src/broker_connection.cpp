#include "./client/include/broker_connection.hpp"
#include <iostream>
#include <nlohmann/json.hpp>
#include "./client/include/hub_manager.hpp"
#include "./client/include/smart_light_proxy.hpp"

namespace asio = boost::asio;
using asio::ip::tcp;

BrokerConnection::BrokerConnection(IBrokerMessageReceiver& receiver, asio::io_context& ctx)
    : m_context(ctx), m_socket(ctx), m_receiver(receiver) {}

BrokerConnection::~BrokerConnection() {
    stop();
}

void BrokerConnection::connect(const std::string& host, int port) {
    try {
        tcp::resolver resolver(m_context);
        auto endpoints = resolver.resolve(host, std::to_string(port));
        boost::system::error_code ec;
        asio::connect(m_socket, endpoints, ec);
        if (ec) {
            throw common::ConnectionError("BrokerConnection: Connect failed: " + ec.message());
        }
        start_read_loop();
    } catch (const std::exception& e) {
        throw common::ConnectionError("BrokerConnection: Connect exception: " + std::string(e.what()));
    }
}

void BrokerConnection::publish(const std::string& topic, const common::NetMessage& msg) {
    nlohmann::json wrapper;
    wrapper["topic"] = topic;
    wrapper["payload"] = msg.serialize();

    send_sync(wrapper.dump() + "\n");
}

void BrokerConnection::subscribe(const std::string& topic) {
    nlohmann::json j;
    j["action"] = "subscribe";
    j["topic"] = topic;
    send_sync(j.dump() + "\n");
}

void BrokerConnection::stop() {
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
        std::cerr << "[BrokerConnection] Error during stop(): " << e.what() << std::endl;
    }
}

void BrokerConnection::start_read_loop() {
    read_handler();

    m_reader_thread = std::thread([this]() {
        try {
            m_context.run();
        } catch (const std::exception& e) {
            std::cerr << "[BrokerConnection] Reader thread crashed: " << e.what() << std::endl;
        }
    });
}

void BrokerConnection::read_handler() {
    asio::async_read_until(
        m_socket, m_read_buffer, "\n", [this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (!ec) {
                std::istream is(&m_read_buffer);
                std::string line;
                std::getline(is, line);

                if (line.empty()) {
                    read_handler();
                    return;
                }

                try {
                    nlohmann::json wrapper = nlohmann::json::parse(line);

                    std::string topic = wrapper.at("topic").get<std::string>();
                    std::string payload_str = wrapper.at("payload").get<std::string>();

                    common::NetMessage msg = common::NetMessage::deserialize(payload_str);

                    m_receiver.onMessageReceived(topic, msg);

                } catch (const std::exception& e) {
                    std::cerr << "[BrokerConnection] Parsing error: " << e.what() << std::endl;
                }

                read_handler();

            } else {
                std::cerr << "[BrokerConnection] Read error: " << ec.message() << std::endl;
            }
        });
}

void BrokerConnection::send_sync(const std::string& message) {
    boost::system::error_code ec;
    asio::write(m_socket, asio::buffer(message), ec);
    if (ec) {
        throw common::ConnectionError("BrokerConnection: Помилка відправки: " + ec.message());
    }
}