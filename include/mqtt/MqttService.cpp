#include "MqttService.hpp"
const std::vector<std::string> TOPICS = {"log", "process", "patch", "tpm", "ids"};

void MqttClient::MqttCallback::message_arrived(mqtt::const_message_ptr msg)
{
    DEBUG("Message received at callback");
    std::string payload = msg->get_payload_str();
    DEBUG(payload.c_str());
    onMessageReceived(msg);
}

void MqttClient::MqttCallback::onMessageReceived(const mqtt::const_message_ptr &msg)
{
    string actionType = mqtt_parser.extractRequestype(msg->get_payload_str());
    string parentType = "";
    int flag = -1;
    if (actionType.empty())
    {
        publisher->sendErrorResponse(msg, SCM::MQTT_REQUEST_EMPTY_OR_NULL_ACTION_TYPE);
        return;
    }
    if (actionType.size() >= 3)
    {
        parentType = actionType.substr(0, 3);
    }

    flag = actionCommand.at(parentType);

    switch (flag)
    {
    case -1:
        publisher->sendErrorResponse(msg, SCM::MQTT_REQUEST_EMPTY_OR_NULL_ACTION_TYPE);
        break;
    case 0:
        handleLog(msg);
        break;
    case 1:
        handleProcess(msg);
        break;
    case 2:
        handlePatch(msg);
        break;
    case 3:
        handleTpm(msg, actionType);
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
            publisher->sendErrorResponse(msg, flag);
            return;
        }

        flag = logHandler->handle(request, logs);

        if (flag == SCM::SUCCESS)
        {
            publisher->sendResponse(request, logs);
        }
        else
        {
            publisher->sendErrorResponse(msg, flag);
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
            publisher->sendErrorResponse(msg, flag);
            return;
        }

        int flag = processHandler->handle(request, logs);

        if (flag == SCM::SUCCESS)
        {
            publisher->sendResponse(request, logs);
        }
        else
        {
            publisher->sendErrorResponse(msg, flag);
        }
    }
    catch (const std::exception &ex)
    {
        std::cerr << ex.what() << '\n';
    }
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
}

void MqttClient::MqttCallback::handleTpm(mqtt::const_message_ptr msg, const string &actionType)
{
    int status = SCM::SUCCESS;
    string errMsg;

    try
    {
        int flag = tpmCommand.at(actionType);
        switch (flag)
        {
        case 0:
        {
            TpmClearRequest request = mqtt_parser.extractTpmClearRequest(msg->get_payload_str(), status, errMsg);
            if (status != SCM::SUCCESS || (status = proxy->validateTpmClearRequest(request)) != SCM::SUCCESS)
            {
                publisher->sendErrorResponse(msg, status);
                return;
            }
            BaseRequest baseRequest = request;
            status = tpmHandler->handle(baseRequest);
            publisher->sendResponse(request, status);
            break;
        }
        case 1:
        {
            TpmClearRequest request = mqtt_parser.extractTpmClearRequest(msg->get_payload_str(), status, errMsg);
            if (status != SCM::SUCCESS || (status = proxy->validateTpmClearRequest(request)) != SCM::SUCCESS)
            {
                publisher->sendErrorResponse(msg, status);
                return;
            }
            status = tpmHandler->handle(request);
            publisher->sendResponse(request, status);
            break;
        }
        default:
            break;
        }
    }
    catch (std::exception &ex)
    {
        std::cerr << ex.what() << '\n';
    }
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