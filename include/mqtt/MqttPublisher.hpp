#ifndef MQTT_PUBLISHER_HPP
#define MQTT_PUBLISHER_HPP

#include "util/Common.hpp"

class MqttPublisher
{

public:
    template <typename T>
    void sendErrorResponse(T request, int status, const string &err_msg)
    {
    }

    void sendAcknowledge(T ack)
    {
    }

    void sendResponse(const std::string &res_type, const std::string &data, const std::string &topic)
    { 
    }

private:
    std::shared_ptr<mqtt::async_client> client = nullptr;
};

#endif