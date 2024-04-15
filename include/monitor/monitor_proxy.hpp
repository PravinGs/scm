#ifndef MONITOR_PROXXY_HPP
#define MONITOR_PROXXY_HPP
#pragma once
#include "util/entity_parser.hpp"
#include "monitor/monitor_service_impl.hpp"
class monitor_proxy : public monitor_service {
public:
    monitor_proxy() : thread_handler(true), service(std::make_unique<monitor_service_impl>()) {}
 
    int get_app_resource_details(const process_entity& entity, vector<process_data>& logs)
    {
        return service->get_app_resource_details(entity, logs);
    }
 
	int get_app_resource_details(config_table_type& config_table) 
	{
        int result = Audit::SUCCESS;
        vector<process_data> logs;
		process_entity entity = parser.get_process_entity(config_table);
        if (!validate_process_entity(entity)) {
            return Audit::FAILED;
        }
        if (entity.time_pattern.empty())
        {
            result = get_app_resource_details(entity, logs);
        }
        else
        {
            schedular = cron::make_cron(entity.time_pattern);
            while (thread_handler)
            {
                common::pause_till_next_execution(schedular);
                result = get_app_resource_details(entity, logs);
                if (result == Audit::FAILED)
                    thread_handler = false;
                result = handle_process_details(logs);
                common::write_log("log_controller: syslog_manager: thread execution done.", Audit::DEBUG);
            }
        }
		return Audit::SUCCESS;
	}
private:
    int handle_process_details(const vector<process_data>& logs) 
    {
        return Audit::SUCCESS;
    }
 
    bool validate_process_entity(process_entity& entity)
    {
        if (!entity.write_path.empty() && os::is_dir_exist(entity.write_path))
        {
            LOG_ERROR("configured write directory path not exist " + entity.write_path);
            //common::write_log("proxy: validate_process_entity: configured write directory path not exist", Audit::FAILED);
            return false;
        }
 
        if (entity.storage_type.empty())
        {
            entity.storage_type = "json";
        }
        return true;
    }
private:
	entity_parser parser;
    cron::cronexpr schedular;
    bool thread_handler;
    std::unique_ptr<monitor_service> service;
};
#endif // !MONITOR_PROXXY_HPP