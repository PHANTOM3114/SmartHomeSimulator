#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "smart_home_exception.hpp" // Підключаємо наші винятки

namespace smarthome::common {

struct NetMessage {
    
    // --- Статичні константи команд ---
    static constexpr const char* CMD_TURN_ON = "cmd:turn_on";
    static constexpr const char* CMD_TURN_OFF = "cmd:turn_off";
    static constexpr const char* CMD_GET_STATUS = "cmd:get_status";
    static constexpr const char* CMD_SET_STATUS = "cmd:set_status";

    // --- Поля даних ---
    std::string command;
    nlohmann::json payload;

    std::string serialize() const {
        nlohmann::json json_data = {
            {"command", this->command},
            {"payload", this->payload}
        };
        // .dump() перетворює json на string
        return json_data.dump(); 
    }

    static NetMessage deserialize(const std::string& raw_data) {
        NetMessage msg;
        try {
            nlohmann::json j = nlohmann::json::parse(raw_data);

            j.at("command").get_to(msg.command);
            j.at("payload").get_to(msg.payload);
            
        } catch (const nlohmann::json::exception& e) {
            // Кидаємо наш кастомний виняток
            throw InvalidMessageError(std::string("Failed to parse NetMessage: ") + e.what());
        }
        return msg;
    }
};

} // namespace smarthome::common