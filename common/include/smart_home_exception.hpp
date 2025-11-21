#pragma once

#include <stdexcept>

namespace common {

class SmartHomeException : public std::runtime_error {
public:
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

}  // namespace common