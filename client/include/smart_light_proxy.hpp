#include <nlohmann/json.hpp>

#include "broker_connection.hpp"

class SmartLightProxy : public IotDevice {
public:
    SmartLightProxy(BrokerConnection& connection, std::string& topic);

    virtual void turn_on() override;
    virtual nlohmann::json get_status() override;

private:
    BrokerConnection& broker_connection;
    std::string topic_base;
    nlohmann::json last_known_status;
};