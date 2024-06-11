#ifndef MQTT_PROCESS_HANDLER_HPP
#define MQTT_PROCESS_HANDLER_HPP

#include "monitor/MonitorProxy.hpp"
#include "util/EntityParser.hpp"

class MqttProcessHandler
{

public:
    MqttProcessHandler(config_table_type &config_table) : configTable(config_table) {}
    int handle(ProcessRequest &request, vector<ProcessData> &logs)
    {
        ProcessEntity entity = entityParser.getProcessEntity(configTable);
        return  monitor.getAppResourceDetails(entity, logs, request.process_names);
    }

private:
    config_table_type configTable;
    MonitorProxy monitor;
    EntityParser entityParser;
};

#endif