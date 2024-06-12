#ifndef MQTT_LOG_HANDLER_HPP
#define MQTT_LOG_HANDLER_HPP

#include "log_collector/LogProxy.hpp"
#include "MqttParser.hpp"

using SCM::SUCCESS;

class MqttLogHandler
{
public:
    MqttLogHandler(config_table_type &configTable) : configTable(configTable), logService(std::make_unique<LogProxy>(configTable))
    {
    }

    int handle(LogRequest& request, vector<string>& logs)
    {
        int result = SUCCESS;
        try
        {
            LogEntity entity = entityParser.getLogEntity(configTable, request.logType); /*Create entity for log collection */

            updateEntity(entity, request);

            if (request.logType == "syslog")
            {
                entity.read_path = SCM::Default::SYSLOG_READ_PATH;
                result = logService->getSysLog(entity, logs);
            }
            else if (request.logType == "auth")
            {
                entity.read_path = SCM::Default::AUTHLOG_READ_PATH;
                result = logService->getSysLog(entity, logs);
            }
            else if (request.logType == "dpkg")
            {
                entity.read_path = SCM::Default::DPKGLOG_READ_PATH;
                result = logService->getSysLog(entity, logs);
            }
            else if (request.logType == "applog")
            {
                result = logService->getAppLog();
            }
            else
            {
                LOG_ERROR("INVALID_MQTT_REQUEST");
                result = SCM::MQTT_REQUEST_INVALID_LOG_TYPE;
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
        return result;
    }

private:
    void updateEntity(LogEntity &entity, const LogRequest &request)
    {
        entity.name = request.logType;

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
    }

private:
    config_table_type configTable;
    std::unique_ptr<LogService> logService;
    MqttParser mqttParser;
    EntityParser entityParser;
};

#endif