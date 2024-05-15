#ifndef MQTT_SERVICE_HPP
#define MQTT_SERVICE_HPP

#include "util/entity_parser.hpp"
#include "log_collector/LogProxy.hpp"
#include "rest/RestService.hpp"
#include "monitor/MonitorProxy.hpp"
#include "patch/PatchProxy.hpp"
#include "tpm/tpm_service.hpp"
#include "MqttParser.hpp"

#define INVALID_MQTT_REQUEST 101

class MqttService 
{
    public:
        virtual void start() = 0;
        virtual ~MqttService() {}
};

class MqttClient : public MqttService
{
public:
    MqttClient() {}
    
    MqttClient(const MqttEntity &entity, config_table_type &config_table)
        : entity(entity),
          config_table(config_table)
    {
        client = std::make_shared<mqtt::async_client>(entity.conn_string, entity.client_id);
        callback = std::make_shared<MqttCallback>(this);
        client->set_callback(*callback);
    }

    void start();

    class MqttCallback : public mqtt::callback
    {
    public:
        MqttCallback(MqttClient* client)
            : client(client)
        {
            log = std::make_unique<LogProxy>(client->config_table);
            r_entity = e_parser.getRestEntity(client->config_table);
        }

        void messageArrived(mqtt::const_message_ptr msg);

    private:
        void publishMqttResponse(const std::string &topic, const std::string &message);
        void sendResponse(const std::string &res_type, const std::string &data, const std::string &topic);
        void onMessageReceived(const mqtt::const_message_ptr &msg);
        void handleLog(mqtt::const_message_ptr msg);
        void handleProcess(mqtt::const_message_ptr msg);
        void handlePatch(mqtt::const_message_ptr msg);
        void handleTpm(mqtt::const_message_ptr msg);

        MqttClient* client;
        std::unique_ptr<LogProxy> log;
        MonitorProxy monitor;
        PatchProxy patch;
        // tpm tpm_service;
        RestEntity r_entity;
        entity_parser e_parser;
        mq parser;
    };

private:
    std::shared_ptr<mqtt::async_client> client = nullptr;
    MqttEntity entity;
    std::map<std::string, std::map<std::string, std::string>> config_table;
    std::shared_ptr<MqttCallback> callback = nullptr;
};

#endif
