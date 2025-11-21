#pragma once

#include <string>
#include "net_message.hpp"

class IBrokerMessageReceiver {
public:
    virtual ~IBrokerMessageReceiver() = default;

    virtual void onMessageReceived(const std::string& topic, const common::NetMessage& msg) = 0;
};