#ifndef MQTT_PROXY_HPP
#define MQTT_PROXY_HPP

#include "MqttService.hpp"

class MqttProxy : public MqttService
{
public:
    MqttProxy(config_table_type& config_table) : mqttService(std::make_unique<MqttClient>())
    void start() 
    {

    }
private:
    bool validateMqttEntity(MqttEntity &entity)
    {
        if (entity.conn_string.empty())
        {
            LOG_ERROR("connection string is empty");
            return false;
        }

        if (entity.topics.empty())
        {
            LOG_ERROR("topics not configured");
            return false;
        }

        if (!entity.ca_cert_path.empty() && !os::isFileExist(entity.ca_cert_path))
        {
            LOG_ERROR("file not exist " + entity.ca_cert_path);
            return false;
        }
        else
        {
            entity.is_secure = false;
        }

        // it is an optional but may be required if server wanted to verify the client
        if (!entity.client_cert_path.empty() && !os::isFileExist(entity.client_cert_path))
        {
            LOG_ERROR("file not exist " + entity.client_cert_path);
        }

        if (entity.client_id.empty())
        {
            entity.client_id = os::host_name;
        }
        return true;
    }

private:
    config_table_type config_table;
    std::unique_ptr<MqttService> mqttService;

};

#endif