#include "json_load_config.hpp"
#include <fstream>

common::JsonLoadConfig::JsonLoadConfig(const std::string& path) {

    std::ifstream file(path);
    if (!file.is_open()) {
        throw common::ConfigurationError("Не вдалося відкрити файл конфігурації: " + path);
    }

    try {
        file >> m_configData;

    } catch (const nlohmann::json::parse_error& e) {

        throw common::ConfigurationError("Помилка парсингу JSON у файлі " + path + ": " + e.what());
    }
}

nlohmann::json common::JsonLoadConfig::getConfig() const {
    return m_configData;
}