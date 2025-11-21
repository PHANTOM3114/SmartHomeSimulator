#include <string>
#include <vector>

#include "common/include/ibroker_message_receiver.hpp"
#include "common/include/iot_device.hpp"
#include "common/include/json_load_config.hpp"
#include "common/include/net_message.hpp"

#include "broker_connection.hpp"

class SmartLightProxy;

class HubManager : public IBrokerMessageReceiver {
public:
    explicit HubManager(const std::string& config_path);
    void run();
    void triggerDeviceOn(int index);

    void onMessageReceived(const std::string& topic, const common::NetMessage& msg) override;

private:
    boost::asio::io_context m_context;
    common::JsonLoadConfig m_config;
    std::unique_ptr<BrokerConnection> m_broker_conn;

    std::string m_brokerIp;
    int m_brokerPort;
    std::vector<std::unique_ptr<IotDevice>> m_devices;
};