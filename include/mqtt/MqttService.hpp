#ifndef MQTT_SERVICE_HPP
#define MQTT_SERVICE_HPP

#include "util/EntityParser.hpp"
#include "log_collector/LogProxy.hpp"
#include "rest/RestService.hpp"
#include "monitor/MonitorProxy.hpp"
#include "patch/PatchProxy.hpp"
#include "tpm/TpmService.hpp"
#include "MqttParser.hpp"
#include "MqttPublisher.hpp"
#include "MqttProxy.hpp"
#include "MqttLogHandler.hpp"
#include "MqttProcessHandler.hpp"
#include "MqttTpmHandler.hpp"

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

    MqttClient(const MqttEntity &entity, const config_table_type &config_table)
        : entity(entity),
          config_table(config_table),
          client(std::make_shared<mqtt::async_client>(entity.conn_string, entity.client_id)),
          callback(std::make_shared<MqttCallback>(this))
    {
        client->set_callback(*callback);
    }
    void start();

    class MqttCallback : public mqtt::callback
    {
    public:
        MqttCallback(config_table_type &config_table) : configTable(config_table)
        {
            logHandler = std::make_unique<MqttLogHandler>(config_table);
            processHandler = std::make_unique<MqttProcessHandler>(config_table);
            r_entity = entity_parser.getRestEntity(config_table);
        }

        MqttCallback(MqttClient *client)
            : client(client),
              publisher(std::make_unique<MqttPublisher>(client->client)),
              logHandler(std::make_unique<MqttLogHandler>(client->config_table)),
              processHandler(std::make_unique<MqttProcessHandler>(client->config_table)),
              tpmHandler(std::make_unique<MqttTpmHandler>())
        {
            r_entity = entity_parser.getRestEntity(client->config_table);
        }

        void message_arrived(mqtt::const_message_ptr msg);

    private:
        void onMessageReceived(const mqtt::const_message_ptr &msg);
        void handleLog(mqtt::const_message_ptr msg);
        void handleProcess(mqtt::const_message_ptr msg);
        void handlePatch(mqtt::const_message_ptr msg);
        void handleTpm(mqtt::const_message_ptr msg, const string& actionType);

        config_table_type configTable;
        MqttClient *client;
        std::unique_ptr<MqttPublisher> publisher;
        std::unique_ptr<MqttLogHandler> logHandler;
        std::unique_ptr<MqttProcessHandler> processHandler;
        std::unique_ptr<MqttTpmHandler> tpmHandler;
        std::unique_ptr<MqttProxy> proxy;
        PatchProxy patch;
        // TpmService tpm_service;
        RestEntity r_entity;
        EntityParser entity_parser;
        MqttParser mqtt_parser;
    };

private:
    MqttEntity entity;
    config_table_type config_table;
    std::shared_ptr<mqtt::async_client> client;
    std::shared_ptr<MqttCallback> callback;
};

#endif
