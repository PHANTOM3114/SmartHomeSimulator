
#include <nlohmann/json_fwd.hpp>

class IotDevice {
public:
    virtual void turn_on() = 0;
    virtual void turn_off() = 0;
    virtual nlohmann::json get_status() = 0;
};