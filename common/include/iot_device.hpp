#pragma once

#include <nlohmann/json.hpp>
#include "net_message.hpp"

class IotDevice {
public:
    virtual ~IotDevice() = default;

    virtual void turn_on() = 0;
    virtual void turn_off() = 0;
    virtual nlohmann::json get_status() = 0;

    virtual std::string getTopic() const = 0;
    virtual void handleIncomingMessage(const common::NetMessage& msg) = 0;
};