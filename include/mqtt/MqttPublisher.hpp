#ifndef MQTT_PUBLISHER_HPP
#define MQTT_PUBLISHER_HPP

#include "MqttStringBuilder.hpp"
#include "util/Common.hpp"

class MqttPublisher
{
public:
    MqttPublisher(const std::shared_ptr<mqtt::async_client> &client);
    void sendErrorResponse(const mqtt::const_message_ptr &request, int status);
    void sendAcknowledge(MqttAck ack);
    void sendResponse(const std::string &res_type, const std::string &data, const std::string &topic);
    template<typename T1, typename T2>
    void sendResponse(const T1& request, T2& data);
private:
    void publish(const std::string &topic, const std::string &message);
private:
    const string ERROR_TOPIC = "error";
    std::shared_ptr<mqtt::async_client> client = nullptr;
    MqttStringBuilder stringBuilder;
};

#endif