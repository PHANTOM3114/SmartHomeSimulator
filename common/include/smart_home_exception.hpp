#pragma once

#include <stdexcept>

namespace smarthome::common {

class SmartHomeException : public std::runtime_error {
public:
    // Використовуємо конструктор батьківського класу
    using std::runtime_error::runtime_error;
};

class InvalidMessageError : public SmartHomeException {
public:
    using SmartHomeException::SmartHomeException;
};

class ConfigurationError : public SmartHomeException {
public:
    using SmartHomeException::SmartHomeException;
};

class ConnectionError : public SmartHomeException {
public:
    using SmartHomeException::SmartHomeException;
};

} // namespace smarthome::common