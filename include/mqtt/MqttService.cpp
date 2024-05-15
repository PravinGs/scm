#include "MqttService.hpp"
const std::vector<std::string> TOPICS = {"log", "process", "patch", "tpm", "ids"};

void MqttClient::MqttCallback::messageArrived(mqtt::const_message_ptr msg)
{
    std::string payload = msg->get_payload_str();
    std::cout << "Received message: " << payload << '\n';
    onMessageReceived(msg);
}

void MqttClient::MqttCallback::publishMqttResponse(const std::string &topic, const std::string &message)
{
    mqtt::message_ptr response_msg = mqtt::make_message(topic, message);
    client->client->publish(response_msg);
}

void MqttClient::MqttCallback::sendResponse(const std::string &res_type, const std::string &data, const std::string &topic)
{
    if (res_type == "MqttResponse")
    {
        publishMqttResponse(topic, data);
    }
    else if (res_type == "RestApiResponse")
    {
        RestResponse response;
        if (response.http_code == SCM::Rest::POST_SUCCESS)
        {
            DEBUG("Syslog data sent to rest api");
        }
    }
}

void MqttClient::MqttCallback::onMessageReceived(const mqtt::const_message_ptr &msg)
{
    int act_type = parser.extractRequestype(msg->get_payload_str());

    switch (static_cast<ActionType>(act_type))
    {
    case ActionType::LogRequest:
        handleLog(msg);
        break;
    case ActionType::PatchRequest:
        handlePatch(msg);
        break;
    case ActionType::ProcessRequest:
        handleProcess(msg);
        break;
    case ActionType::TpmConfigServiceuration:
        handleTpm(msg);
        break;
    default:
        break;
    }
}

void MqttClient::MqttCallback::handleLog(mqtt::const_message_ptr msg)
{
    vector<std::string> logs;
    string json_string = "";
    int result = SUCCESS;

    LogRequest request = parser.extractLogRequest(msg->get_payload_str()); /* Extract json string and converted to respective request*/

    LogEntity entity = e_parser.getLogEntity(client->config_table, request.log_type); /*Create entity for log collection */

    if (request.log_type == "syslog")
    {
        if (!request.start_time.empty())
        {
            entity.last_read_time = Common::stringToTime(request.start_time);
        }
        result = log->getSysLog(entity, logs);
    }
    else if (request.log_type == "applog")
    {
        result = log->getAppLog();
    }
    else
    {
        result = INVALID_MQTT_REQUEST;
    }
    json_string = parser.logToJSON(logs, entity.name);

    sendResponse(parser.responseTypeToString(request.ResponseType), json_string, request.source_id);
}

void MqttClient::MqttCallback::handleProcess(mqtt::const_message_ptr msg)
{
    vector<ProcessData> logs;
    string json_string = "";
    ProcessRequest request = parser.extractProcessRequest(msg->get_payload_str());

    ProcessEntity entity = e_parser.getProcessEntity(client->config_table);

    int result = monitor.getAppResourceDetails(entity, logs);

    json_string = parser.ProcessToJSON(logs);

    sendResponse(parser.responseTypeToString(request.ResponseType), json_string, request.source_id);
}

void MqttClient::MqttCallback::handlePatch(mqtt::const_message_ptr msg)
{
    string json_string = "";

    PatchRequest request = parser.extractPatchRequest(msg->get_payload_str());

    PatchEntity entity = e_parser.getPatchEntity(client->config_table);

    patch.start(entity);

    sendResponse(parser.responseTypeToString(request.ResponseType), json_string, request.source_id);
}

void MqttClient::MqttCallback::handleTpm(mqtt::const_message_ptr msg)
{
    string json_string = "";
    TPM2_RC result = TPM2_SUCCESS;
    TpmRequest request = parser.extractTpmRequest(msg->get_payload_str());
    TpmConfig ConfigService;
    SealConfig s_ConfigService;
    PersistConfig p_ConfigService;

    switch (request.command)
    {
    case 0:
        ConfigService = parser.getTpmConfig(request, msg->get_payload_str());
        result = TPM2_SUCCESS /*tpm_service.clear_tpm(ConfigService.lockout_auth.c_str())*/;
        if (result == TPM2_SUCCESS)
        {
            json_string = "Tpm_clear: success";
        }
        break;
    case 1:
        s_ConfigService = parser.getSealConfig(request, msg->get_payload_str());
        break;
    case 2:
        s_ConfigService = parser.getSealConfig(request, msg->get_payload_str());
        break;
    case 3:
        p_ConfigService = parser.getTpmPersistContext(request, msg->get_payload_str());
        break;
    case 4:
        p_ConfigService = parser.getTpmPersistContext(request, msg->get_payload_str());
        break;
    default:
        LOG_ERROR("Unexpected TpmCommand value: ", request.command);
        break;
    }

    sendResponse(parser.responseTypeToString(request.ResponseType), json_string, request.source_id);
}

void MqttClient::start()
{
    mqtt::connect_options conn_opts = mqtt::connect_options_builder().clean_session(true).finalize();
    try
    {

        DEBUG(client->get_client_id() + " : connecting to the mqtt server");
        client->connect(conn_opts)->wait();
        for (const string &topic : entity.topics) // It can be replaced with entity.topics
        {
            client->subscribe(topic, entity.qos)->wait();
            DEBUG("MqttClient: start: " + entity.client_id + ": subscribe to " + topic);
        }

        while (true)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        if (client->is_connected())
        {
            DEBUG(entity.client_id + " : Shutting down and disconnecting from the MQTT server...");
            for (const auto &topic : entity.topics)
            {
                client->unsubscribe(topic)->wait();
            }
            client->stop_consuming();
            client->disconnect()->wait();
        }
        else
        {
            DEBUG(entity.client_id + " disconnected");
        }
    }

    catch (const mqtt::exception &e)
    {
        string error = e.what();
        LOG_ERROR("mqtt_service excepiton: " + error);
        std::cerr << e.what() << '\n';
    }
}