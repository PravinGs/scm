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
        }
        else
        {
            LOG_ERROR("Failed to send to rest API");
        }
    }
}

void MqttClient::MqttCallback::onMessageReceived(const mqtt::const_message_ptr &msg)
{
    int act_type = mqtt_parser.extractRequestype(msg->get_payload_str());

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
    int flag = SUCCESS;
    string error;
    try
    {
        LogRequest request = mqtt_parser.extractLogRequest(msg->get_payload_str(), flag, error); /* Extract json string and converted to respective request*/

        if (flag != SCM::SUCCESS || (flag = proxy->validateLogRequest(request)) != SCM::SUCCESS)
        {
            publisher->sendErrorResponse(msg->get_payload_str(), flag);
            return;
        }

        flag = logHandler->handle(request, logs);

        if (flag == SCM::SUCCESS)
        {
            publisher->sendResponse(request, logs);
        }
        else
        {
            publisher->sendErrorResponse(msg->get_payload_str(), flag);
            return;
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
    int flag = SCM::SUCCESS;
    string error;
    try
    {
        ProcessRequest request = mqtt_parser.extractProcessRequest(msg->get_payload_str(), flag, error);
        if (flag != SCM::SUCCESS || (flag = proxy->validateProcessRequest(request)) != SCM::SUCCESS)
        {
            publisher->sendErrorResponse(msg->get_payload_str(), flag);
            return;
        }

        int flag = processHandler->handle(request, logs);

        if (flag == SCM::SUCCESS)
        {
            publisher->sendResponse(request, logs);
        }
        else
        {
            publisher->sendErrorResponse(msg->get_payload_str(), flag);
        }
    }
    catch (const std::exception &ex)
    {
        std::cerr << ex.what() << '\n';
    }

    // int result = monitor.getAppResourceDetails(entity, logs, request.process_names);

    // json_string = mqtt_parser.ProcessToJSON(logs);

    // sendResponse(mqtt_parser.responseTypeToString(request.responseType), json_string, request.sourceId);
}

void MqttClient::MqttCallback::handlePatch(mqtt::const_message_ptr msg)
{
    string json_string = "";

    PatchRequest request = mqtt_parser.extractPatchRequest(msg->get_payload_str());

    PatchEntity entity = entity_parser.getPatchEntity(client->config_table);

    if (!request.url.empty())
    {
        entity.url = request.url;
    }
    if (!request.username.empty())
    {
        entity.username = request.username;
    }
    if (!request.password.empty())
    {
        entity.password = request.password;
    }
    patch.start(entity);
    // sendResponse(mqtt_parser.responseTypeToString(request.responseType), json_string, request.sourceId);
}

void MqttClient::MqttCallback::handleTpm(mqtt::const_message_ptr msg)
{
    string json_string = "";
    TPM2_RC result = TPM2_SUCCESS;
    TpmRequest request = mqtt_parser.extractTpmRequest(msg->get_payload_str());
    TpmConfig ConfigService;
    SealConfig s_ConfigService;
    PersistConfig p_ConfigService;

    switch (request.command)
    {
    case 0:
        ConfigService = mqtt_parser.getTpmConfig(request, msg->get_payload_str());
        result = TPM2_SUCCESS /*tpm_service.clear_tpm(ConfigService.lockout_auth.c_str())*/;
        if (result == TPM2_SUCCESS)
        {
            json_string = "Tpm_clear: success";
        }
        break;
    case 1:
        s_ConfigService = mqtt_parser.getSealConfig(request, msg->get_payload_str());
        break;
    case 2:
        s_ConfigService = mqtt_parser.getSealConfig(request, msg->get_payload_str());
        break;
    case 3:
        p_ConfigService = mqtt_parser.getTpmPersistContext(request, msg->get_payload_str());
        break;
    case 4:
        p_ConfigService = mqtt_parser.getTpmPersistContext(request, msg->get_payload_str());
        break;
    default:
        LOG_ERROR("Unexpected TpmCommand value: ", request.command);
        break;
    }

    sendResponse(mqtt_parser.responseTypeToString(request.responseType), json_string, request.sourceId);
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