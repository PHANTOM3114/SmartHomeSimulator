#include <string>

#include <nlohmann/json.hpp>
#include "common/include/smart_home_exception.hpp"

class JsonLoadConfig {
public:
    explicit JsonLoadConfig(const std::string& path);
    void load_config(std::string& path);

    nlohmann::json getConfig() const;

    template<typename T>
    T getValue(const std::string& key) const {
        try {
            // .at(key) - шукає ключ
            // .get<T>() - намагається конвертувати в тип T
            return m_configData.at(key).get<T>();
        
        } catch (const nlohmann::json::exception& e) {
            // Якщо ключ не знайдено (.at) або тип невірний (.get)
            // Кидаємо *твій* кастомний виняток
            throw smarthome::common::ConfigurationError(
                "Ключ не знайдено або невірний тип у конфігу: '" + key + "'. Деталі: " + e.what()
            );
        }
    }

private:
    nlohmann::json m_configData;
};