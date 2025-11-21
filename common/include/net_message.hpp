#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include "smart_home_exception.hpp"

namespace common {

struct NetMessage {

    static constexpr const char* CMD_TURN_ON = "cmd:turn_on";
    static constexpr const char* CMD_TURN_OFF = "cmd:turn_off";
    static constexpr const char* CMD_GET_STATUS = "cmd:get_status";
    static constexpr const char* CMD_SET_STATUS = "cmd:set_status";

    std::string command;
    nlohmann::json payload;

    std::string serialize() const {
        nlohmann::json json_data = {{"command", this->command}, {"payload", this->payload}};
        return json_data.dump();
    }

    static NetMessage deserialize(const std::string& raw_data) {
        NetMessage msg;
        try {
            nlohmann::json j = nlohmann::json::parse(raw_data);

            j.at("command").get_to(msg.command);
            j.at("payload").get_to(msg.payload);

        } catch (const nlohmann::json::exception& e) {
            throw InvalidMessageError(std::string("Failed to parse NetMessage: ") + e.what());
        }
        return msg;
    }
};

}  // namespace common