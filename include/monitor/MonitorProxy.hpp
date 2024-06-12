#ifndef MONITOR_PROXXY_HPP
#define MONITOR_PROXXY_HPP
#pragma once
#include "util/ConfigService.hpp"
#include "util/EntityParser.hpp"
#include "monitor/MonitorServiceImpl.hpp"
#include "rest/RestService.hpp"
#include "MonitorRepository.hpp"

class MonitorProxy : public MonitorService
{
public:
    MonitorProxy() : thread_handler(true), service(std::make_unique<MonitorServiceImpl>())
    {
    }

    MonitorProxy(config_table_type &config_table): thread_handler(true), service(std::make_unique<MonitorServiceImpl>())
    {
        // MonitorProxy();
        r_entity = entity_parser.getRestEntity(config_table);
    }

    int getAppResourceDetails(ProcessEntity &entity, vector<ProcessData> &logs, vector<string> &processNames)
    {
        if (!validateProcessEntity(entity))
        {
            return SCM::FAILED;
        }
        return service->getAppResourceDetails(entity, logs, processNames);
    }

    int getAppResourceDetails(const ProcessEntity &entity, vector<ProcessData> &logs)
    {
        string json_string;
        int res = service->getAppResourceDetails(entity, logs);
        if (res == SCM::SUCCESS)
        {
            SystemProperties properties = getSystemProperties();
            SystemProperties availed_properties = getAvailedSystemProperties();
            json_string = ProcessToJSON(logs, properties, availed_properties);
            RestResponse response = RestService::http_post(r_entity.resources_post_url, json_string);
            DEBUG("Rest Api status: ", std::to_string(response.http_code));

            if (response.http_code != SCM::Rest::POST_SUCCESS && db.save(logs, properties, availed_properties) != SCM::SUCCESS)
            {
                DEBUG("Failed to store system process details locally");
            }
            if (response.http_code == SCM::Rest::POST_SUCCESS)
            {
                INFO("Sent to cloud successfully");
                RestService::start(r_entity, "process"); // see later
            }
        }
        return res;
    }

    int getAppResourceDetails(config_table_type &config_table)
    {
        int result = SCM::SUCCESS;
        vector<ProcessData> logs;
        ProcessEntity entity = entity_parser.getProcessEntity(config_table);
        r_entity = entity_parser.getRestEntity(config_table);
        if (!validateProcessEntity(entity))
        {
            return SCM::FAILED;
        }
        if (entity.time_pattern.empty())
        {
            result = getAppResourceDetails(entity, logs);
        }
        else
        {
            schedular = cron::make_cron(entity.time_pattern);
            while (thread_handler)
            {
                Common::pauseTillNextExecution(schedular);
                result = getAppResourceDetails(entity, logs);
                if (result == SCM::FAILED)
                    thread_handler = false;
                result = handleProcessDetails(logs);
                DEBUG("thread execution done.");
            }
        }
        return SCM::SUCCESS;
    }

    SystemProperties getSystemProperties()
    {
        return service->getSystemProperties();
    }

    SystemProperties getAvailedSystemProperties()
    {
        return service->getAvailedSystemProperties();
    }

    // private:

    int handleProcessDetails(const vector<ProcessData> &logs)
    {
        return SCM::SUCCESS;
    }

    bool validateProcessEntity(ProcessEntity &entity)
    {
        // if (!entity.write_path.empty() && !os::isDirExist(entity.write_path))
        // {
        //     LOG_ERROR("ConfigServiceured write directory path not exist " + entity.write_path);
        //     return false;
        // }

        if (entity.storage_type.empty())
        {
            entity.storage_type = "json";
        }
        return true;
    }

private:
    EntityParser entity_parser;
    cron::cronexpr schedular;
    bool thread_handler;
    std::unique_ptr<MonitorService> service;
    config_table_type config_table;
    ConfigService configService;
    RestEntity r_entity;
    MonitorRepository db;
};
#endif // !MONITOR_PROXXY_HPP