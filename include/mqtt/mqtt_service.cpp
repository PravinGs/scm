#include "mqtt_service.hpp"
const std::vector<std::string> TOPICS = {"log", "process", "patch", "tpm", "ids"};

void mqtt_client::mqtt_callback::message_arrived(mqtt::const_message_ptr msg)
{
    std::string payload = msg->get_payload_str();
    std::cout << "Received message: " << payload << '\n';
    on_message_received(msg);
}

void mqtt_client::mqtt_callback::publish_mqtt_response(const std::string &topic, const std::string &message)
{
    mqtt::message_ptr response_msg = mqtt::make_message(topic, message);
    client->client->publish(response_msg);
}

void mqtt_client::mqtt_callback::send_response(const std::string &res_type, const std::string &data, const std::string &topic)
{
    if (res_type == "MqttResponse")
    { // extract this code into seperate function
        publish_mqtt_response(topic, data);
    }
    else if (res_type == "RestApiResponse")
    {
        // long http_code = rest_service::post(r_entity, data);
        rest_response response;
        if (response.http_code == Audit::Rest::POST_SUCCESS)
        {
            // check at the default dir if log files been stored temproarily
            DEBUG("Syslog data sent to rest api");
        }
    }
}

void mqtt_client::mqtt_callback::on_message_received(const mqtt::const_message_ptr &msg)
{
    int act_type = parser.extract_request_type(msg->get_payload_str());

    switch (static_cast<action_type>(act_type))
    {
    case action_type::LogRequest:
        handle_log(msg);
        break;
    case action_type::PatchRequest:
        handle_patch(msg);
        break;
    case action_type::ProcessRequest:
        handle_process(msg);
        break;
    case action_type::TpmConfiguration:
        handle_tpm(msg);
        break;
    default:
        break;
    }
}

void mqtt_client::mqtt_callback::handle_log(mqtt::const_message_ptr msg)
{
    vector<std::string> logs;
    string json_string = "";
    int result = SUCCESS;

    log_request request = parser.extract_log_request(msg->get_payload_str()); /* Extract json string and converted to respective request*/

    log_entity entity = e_parser.get_log_entity(client->config_table, request.log_type); /*Create entity for log collection */

    if (request.log_type == "syslog")
    {
        if (!request.start_time.empty())
        {
            entity.last_read_time = common::string_to_time_t(request.start_time);
            // entity.read_path = "/var/log/syslog";
        }
        result = log->get_syslog(entity, logs);
    }
    else if (request.log_type == "applog")
    {
        result = log->get_applog(); //
    }
    else
    {
        result = INVALID_MQTT_REQUEST; // Invalid request from the server
    }
    json_string = parser.log_to_json(logs, entity.name);

    // if (result != SUCCESS)
    // {
    //     json_string = "{ status: failed }";
    // }
    // else
    // {
    //     json_string = parser.log_to_json(logs);
    // }
    send_response(parser.response_type_string(request.response_type), json_string, request.source_id);
}

void mqtt_client::mqtt_callback::handle_process(mqtt::const_message_ptr msg)
{
    vector<process_data> logs;
    string json_string = "";
    process_request request = parser.extract_process_request(msg->get_payload_str());

    process_entity entity = e_parser.get_process_entity(client->config_table);

    int result = monitor.get_app_resource_details(entity, logs);

    json_string = parser.process_to_json(logs);

    // if (result == Audit::SUCCESS && !logs.empty())
    // {
    //     json_string = parser.process_to_json(logs);
    // }
    send_response(parser.response_type_string(request.response_type), json_string, request.source_id);
}

// void mqtt_callback::handle_ids()
// {
// }

void mqtt_client::mqtt_callback::handle_patch(mqtt::const_message_ptr msg)
{
    string json_string = "";

    patch_request request = parser.extract_patch_request(msg->get_payload_str());

    patch_entity entity = e_parser.get_patch_entity(client->config_table);

    patch.start(entity);

    send_response(parser.response_type_string(request.response_type), json_string, request.source_id);
}

void mqtt_client::mqtt_callback::handle_tpm(mqtt::const_message_ptr msg)
{
    string json_string = "";
    TPM2_RC result = TPM2_SUCCESS;
    tpm_request request = parser.extract_tpm_request(msg->get_payload_str());
    tpm_config config;
    seal_config s_config;
    persist_config p_config;

    switch (request.command)
    {
    case 0:
        config = parser.get_tpm_config(request, msg->get_payload_str());
        result = TPM2_SUCCESS /*tpm_service.clear_tpm(config.lockout_auth.c_str())*/;
        if (result == TPM2_SUCCESS)
        {
            json_string = "Tpm_clear: success";
        }
        // cout << config.lockout_auth << '\n';
        break;
    case 1:
        s_config = parser.get_seal_config(request, msg->get_payload_str());
        break;
    case 2:
        s_config = parser.get_seal_config(request, msg->get_payload_str());
        break;
    case 3:
        p_config = parser.get_persist_context(request, msg->get_payload_str());
        break;
    case 4:
        p_config = parser.get_persist_context(request, msg->get_payload_str());
        break;
    default:
        // Handle any unexpected enumeration values
        std::cerr << "Error: Unexpected tpm_command value." << '\n';
        break;
    }

    send_response(parser.response_type_string(request.response_type), json_string, request.source_id);
}

void mqtt_client::start()
{
    // client = std::make_shared<mqtt::async_client>(entity.conn_string, entity.client_id);
    // callback = std::make_shared<mqtt_callback>(*this);
    // client->set_callback(*callback);
    mqtt::connect_options conn_opts = mqtt::connect_options_builder().clean_session(true).finalize();
    try
    {

        DEBUG(client->get_client_id() + " : connecting to the mqtt server");
        client->connect(conn_opts)->wait();
        for (const string &topic : entity.topics) // It can be replaced with entity.topics
        {
            client->subscribe(topic, entity.qos)->wait();
            DEBUG("mqtt_client: start: " + entity.client_id + ": subscribe to " + topic);
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