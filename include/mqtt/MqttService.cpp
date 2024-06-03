#include "MqttService.hpp"
const std::vector<std::string> TOPICS = {"log", "process", "patch", "tpm", "ids"};

void MqttClient::MqttCallback::message_arrived(mqtt::const_message_ptr msg)
{
    DEBUG("Message received at callback");
    std::string payload = msg->get_payload_str();
    DEBUG(payload.c_str());
    onMessageReceived(msg);
}

void MqttClient::MqttCallback::publishMqttResponse(const std::string &topic, const std::string &message)
{
    DEBUG("Response is about to publish to topic " + topic);
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
        RestResponse response = RestService::http_post(topic, data);
        DEBUG("Rest Api status: ", std::to_string(response.http_code));
        if (response.http_code == SCM::Rest::POST_SUCCESS)
        {
            DEBUG("Data sent to rest api");
        }else {
            LOG_ERROR("Failed to send to rest API");
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
    DEBUG("Handle Log");
    vector<std::string> logs;
    string json = "";
    int result = SUCCESS;
    try
    {
        LogRequest request = parser.extractLogRequest(msg->get_payload_str()); /* Extract json string and converted to respective request*/

        if (!proxy->validateLogRequest(request))
        {
            return;
        }

        LogEntity entity = e_parser.getLogEntity(configTable, request.logType); /*Create entity for log collection */
        entity.name = request.logType;

        if (request.logType == "syslog")
        {
            entity.read_path = SCM::Default::SYSLOG_READ_PATH;
            if (!request.startDate.empty())
            {
                entity.last_read_time = Common::utcStringToTime(request.startDate); // Todo This check have not added there
            }
            
            if (!request.endDate.empty())
            {
                entity.end_time = Common::utcStringToTime(request.endDate); // Todo This check have not added there
            }
            else
            {
                entity.end_filter = true;
            }

            if (!request.logLevels.empty())
            {
                entity.log_levels = request.logLevels;
            }
            result = log->getSysLog(entity, logs);
        }
        else if (request.logType == "auth")
        {
            entity.read_path = SCM::Default::AUTHLOG_READ_PATH;
            if (!request.startDate.empty())
            {
                entity.last_read_time = Common::utcStringToTime(request.startDate);
            }

            if (!request.endDate.empty())
            {
                entity.end_time = Common::utcStringToTime(request.endDate);
            }
            result = log->getSysLog(entity, logs);
        }
        else if (request.logType == "dpkg")
        {
            entity.read_path = SCM::Default::DPKGLOG_READ_PATH;
            if (!request.startDate.empty())
            {
                entity.last_read_time = Common::utcStringToTime(request.startDate); // Todo This check have not added there
            }
            result = log->getSysLog(entity, logs);
            for (const std::string &log : logs)
            {
                std::cout << log << '\n';
            }
        }
        else if (request.logType == "applog")
        {
            result = log->getAppLog();
        }
        else
        {
            LOG_ERROR("INVALID_MQTT_REQUEST");
            result = INVALID_MQTT_REQUEST;
        }
        if (result == SCM::SUCCESS)
        {
            const int batch_size = 100;
            if (logs.size() == 0)
            {
                json = parser.logToJSON(logs, entity.name);
                sendResponse(parser.responseTypeToString(request.responseType), json, request.sourceId); return;
            }
            for (size_t i = 0; i < logs.size(); i += batch_size)
            {
                size_t end = std::min(i + batch_size, logs.size());
                std::vector<std::string> batch(logs.begin() + i, logs.begin() + end);
                json = parser.logToJSON(batch, entity.name);
                sendResponse(parser.responseTypeToString(request.responseType), json, request.sourceId);
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

void MqttClient::MqttCallback::handleProcess(mqtt::const_message_ptr msg)
{
    vector<ProcessData> logs;
    string json_string = "";
    ProcessRequest request = parser.extractProcessRequest(msg->get_payload_str());

    ProcessEntity entity = e_parser.getProcessEntity(client->config_table);

    int result = monitor.getAppResourceDetails(entity, logs); 

    // while ()
    // {
       
    // }
    

    json_string = parser.ProcessToJSON(logs);

    sendResponse(parser.responseTypeToString(request.responseType), json_string, request.sourceId);
}

void MqttClient::MqttCallback::handlePatch(mqtt::const_message_ptr msg)
{
    string json_string = "";

    PatchRequest request = parser.extractPatchRequest(msg->get_payload_str());

    PatchEntity entity = e_parser.getPatchEntity(client->config_table);

    patch.start(entity);

    sendResponse(parser.responseTypeToString(request.responseType), json_string, request.sourceId);
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

    sendResponse(parser.responseTypeToString(request.responseType), json_string, request.sourceId);
}

void MqttClient::start()
{
    mqtt::connect_options conn_opts = mqtt::connect_options_builder().clean_session(true).finalize();
    try
    {
        DEBUG(client->get_client_id() + " : connecting to the mqtt server");
        client->connect(conn_opts); //->wait();

        while (!client->is_connected())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        DEBUG(client->get_client_id() + " : connected to the mqtt server");

        // client->subscribe(entity.topics, entity.qos); // qos=1
        for (const string &topic : entity.topics) // It can be replaced with entity.topics
        {
            client->subscribe(topic, entity.qos); // wait();
            DEBUG(entity.client_id + ": subscribe to " + topic);
        }

        while (client->is_connected())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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