#include "MqttPublisher.hpp"

MqttPublisher::MqttPublisher(const std::shared_ptr<mqtt::async_client> &client) : client(client) {}

void MqttPublisher::publish(const std::string &topic, const std::string &message)
{
    DEBUG("Response is about to publish to topic " + topic);
    mqtt::message_ptr response_msg = mqtt::make_message(topic, message);
    client->publish(response_msg);
}

void MqttPublisher::sendErrorResponse(const mqtt::const_message_ptr &request, int status)
{
    DEBUG("publishing error response to topic error");
    MqttErrorReponse errorResponse;
    errorResponse.status = status;
    errorResponse.errMsg = Common::getErrorString(status);

    string err_msg;
    Json::Value root;
    Json::CharReaderBuilder builder;
    std::istringstream iss(request->get_payload_str());

    Json::parseFromStream(builder, iss, &root, &err_msg);

    if (!err_msg.empty())
    {
        errorResponse.payload = request->get_payload();
    }
    else
    {
        errorResponse.payload = root;
    }
    mqtt::message_ptr response_msg = mqtt::make_message(ERROR_TOPIC, stringBuilder.toJson(errorResponse));
    client->publish(response_msg);
}

void MqttPublisher::sendAcknowledge(MqttAck ack)
{
}

template <>
void MqttPublisher::sendResponse(const LogRequest &request, std::vector<string> &logs)
{
    const int batch_size = 100;
    MqttResponse response(request.id, request.actionType);
    if (logs.size() == 0)
    {
        response.result = stringBuilder.toJson(logs, request.logType);
        string json = stringBuilder.toJson(response);
        publish(request.sourceId, json);
        return;
    }
    for (size_t i = 0; i < logs.size(); i += batch_size)
    {
        size_t end = std::min(i + batch_size, logs.size());
        std::vector<std::string> batch(logs.begin() + i, logs.begin() + end);
        response.result = stringBuilder.toJson(batch, request.logType);
        string json = stringBuilder.toJson(response);
        publish(request.sourceId, json);
    }
}

template <>
void MqttPublisher::sendResponse(const ProcessRequest &request, std::vector<ProcessData> &logs)
{
    const int batch_size = 100;
    MqttResponse response(request.id, request.actionType);
    if (logs.size() == 0)
    {
        response.result = stringBuilder.toJson(logs);
        string json = stringBuilder.toJson(response);
        publish(request.sourceId, json);
        return;
    }
    for (size_t i = 0; i < logs.size(); i += batch_size)
    {
        size_t end = std::min(i + batch_size, logs.size());
        std::vector<ProcessData> batch(logs.begin() + i, logs.begin() + end);
        response.result = stringBuilder.toJson(batch);
        string json = stringBuilder.toJson(response);
        publish(request.sourceId, json);
    }
}

template <>
void MqttPublisher::sendResponse(const TpmClearRequest &request, int &status)
{
    MqttResponse response(request.id, request.actionType);
    response.errMsg = Common::getErrorString(status);
    string json = stringBuilder.toJson(response);
    publish(request.sourceId, json);
}