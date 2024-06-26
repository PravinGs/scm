#ifndef MQTT_PROXY_HPP
#define MQTT_PROXY_HPP

#include "MqttService.hpp"

class MqttProxy
{
private:
    int validateBaseRequest(const string &id, const string &actionType, const int responseType, const string &topic)
    {
        if (id.empty())
        {
            return SCM::MQTT_REQUEST_EMPTY_OR_NULL_REQUEST_ID;
        }
        if (actionType.empty())
        {
            return SCM::MQTT_REQUEST_EMPTY_OR_NULL_ACTION_TYPE;
        }
        if (responseType < 0 || responseType > 1)
        {
            return SCM::MQTT_REQUEST_INVALID_RESPONSE_TYPE;
        }
        if (topic.empty())
        {
            return SCM::MQTT_REQUEST_EMPTY_OR_NULL_TOPIC_ID;
        }
        else
        {
            return SCM::SUCCESS;
        }
    }

public:
    int validateLogRequest(const LogRequest &request)
    {
        // int isValidBaseRequest =
        // if (isValidBaseRequest != SCM::SUCCESS)
        // {
        //     return isValidBaseRequest;
        // }
        return validateBaseRequest(request.id, request.actionType, request.responseType, request.topic);
        ;
    }

    int validateProcessRequest(const ProcessRequest &request)
    {
        return validateBaseRequest(request.id, request.actionType, request.responseType, request.topic);
    }

    int validateTpmClearRequest(const TpmClearRequest &request)
    {
        return validateBaseRequest(request.id, request.actionType, request.responseType, request.topic);
    }
    int validateTpmRequest(const TpmRequest &request)
    {
        return validateBaseRequest(request.id, request.actionType, request.responseType, request.topic);
    }
    int validateTpmChangePasswordRequest(const TpmChangePasswordRequest &request)
    {
        return validateBaseRequest(request.id, request.actionType, request.responseType, request.topic);
    }
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
};

#endif