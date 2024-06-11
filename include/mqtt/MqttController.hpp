#ifndef MQTT_CONTROLLER_HPP
#define MQTT_CONTROLLER_HPP

#include "MqttService.hpp"
#include "MqttProxy.hpp"
#include "util/ConfigService.hpp"
#include "util/EntityParser.hpp"
class MqttController
{
public:
    MqttController(const config_table_type &config_table) : config_table(config_table), is_valid_config(true) {}
    
    MqttController(const string &config_file)
    {
        is_valid_config = (config.readIniConfigFile(config_file, config_table) != SCM::SUCCESS) ? false : true;
    }

    void start()
    {
        if (!is_valid_config)
        {
            return;
        }
        MqttEntity entity = entity_parser.getMqttEntity(config_table);
        if (!proxy->validateMqttEntity(entity))
        {
            return;
        }
        service = std::make_unique<MqttClient>(entity, config_table);
        service->start();
    }
   
    ~MqttController()
    {
    }

private:
    std::unique_ptr<MqttService> service = nullptr;
    std::unique_ptr<MqttProxy> proxy;
    ConfigService config;
    EntityParser entity_parser;
    config_table_type config_table;
    bool is_valid_config;
    
};

#endif
