#include "json_load_config.hpp"
#include <fstream> 

JsonLoadConfig::JsonLoadConfig(const std::string& path) {
    
    std::ifstream file(path);
    if (!file.is_open()) {
        throw smarthome::common::ConfigurationError("Не вдалося відкрити файл конфігурації: " + path);
    }

    try {
        file >> m_configData;
    
    } catch (const nlohmann::json::parse_error& e) {

        throw smarthome::common::ConfigurationError("Помилка парсингу JSON у файлі " + path + ": " + e.what());
    }
}

nlohmann::json JsonLoadConfig::getConfig() const {
    return m_configData;
}