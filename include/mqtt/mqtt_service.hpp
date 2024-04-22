#ifndef MQTT_SERVICE_HPP
#define MQTT_SERVICE_HPP

#include "util/entity_parser.hpp"
#include "log_collector/log_proxy.hpp"
#include "rest/rest_service.hpp"
#include "monitor/monitor_proxy.hpp"
#include "patch/patch_proxy.hpp"
#include "tpm/tpm_service.hpp"
#include "mqtt_parser.hpp"

#define INVALID_MQTT_REQUEST 101

class mqtt_client
{
public:
    mqtt_client() {}
    mqtt_client(const mqtt_entity &entity, config_table_type &config_table)
        : entity(entity),
          config_table(config_table)
    {
        client = std::make_shared<mqtt::async_client>(entity.conn_string, entity.client_id);
        callback = std::make_shared<mqtt_callback>(this);
        client->set_callback(*callback);
    }

    void start();

    class mqtt_callback : public mqtt::callback
    {
    public:
        mqtt_callback(mqtt_client* client)
            : client(client)
        {
            log = std::make_unique<log_proxy>(client->config_table);
            r_entity = e_parser.get_rest_entity(client->config_table);
        }

        void message_arrived(mqtt::const_message_ptr msg);

    private:
        void publish_mqtt_response(const std::string &topic, const std::string &message);
        void send_response(const std::string &res_type, const std::string &data, const std::string &topic);
        void on_message_received(const mqtt::const_message_ptr &msg);
        void handle_log(mqtt::const_message_ptr msg);
        void handle_process(mqtt::const_message_ptr msg);
        void handle_patch(mqtt::const_message_ptr msg);
        void handle_tpm(mqtt::const_message_ptr msg);

        mqtt_client* client;
        std::unique_ptr<log_proxy> log;
        monitor_proxy monitor;
        patch_proxy patch;
        // tpm tpm_service;
        rest_entity r_entity;
        entity_parser e_parser;
        mq parser;
    };

private:
    std::shared_ptr<mqtt::async_client> client = nullptr;
    mqtt_entity entity;
    std::map<std::string, std::map<std::string, std::string>> config_table;
    std::shared_ptr<mqtt_callback> callback = nullptr;
};

#endif
