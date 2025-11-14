#pragma once

#include "net_message.hpp"
#include <string>

class IBrokerMessageReceiver {
public:
    virtual ~IBrokerMessageReceiver() = default;

    virtual void onMessageReceived(const std::string& topic, const common::NetMessage& msg) = 0;
};