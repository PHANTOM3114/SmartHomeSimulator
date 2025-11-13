#include <string>
#include <memory>
#include <vector>

#include "common/include/iot_deivce.hpp"
#include "common/include/json_load_config.hpp"

class HubManager {
public:
    HubManager(std::string& config_path);

private:
    JsonLoadConfig m_config;
    std::vector<std::unique_ptr<IotDevice>> devices;
};
    