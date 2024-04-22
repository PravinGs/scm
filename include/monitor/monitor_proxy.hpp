#ifndef MONITOR_PROXXY_HPP
#define MONITOR_PROXXY_HPP
#pragma once
#include "util/config.hpp"
#include "util/entity_parser.hpp"
#include "monitor/monitor_service_impl.hpp"
#include "rest/rest_service.hpp"
#include "monitor_repository.hpp"

class monitor_proxy : public monitor_service
{
public:
    monitor_proxy(): thread_handler(true), service(std::make_unique<monitor_service_impl>()) 
    {

    }

    monitor_proxy(config_table_type &config_table) 
    {
        monitor_proxy();
        r_entity = parser.get_rest_entity(config_table);
    }

    int get_app_resource_details(const process_entity &entity, vector<process_data> &logs)
    {
        string json_string;
        int res = service->get_app_resource_details(entity, logs);

        if (res == Audit::SUCCESS)
        {
            // for (const process_data &data : logs)
            // {
            //     std::cout << "Process ID: " << data.process_id << " "
            //               << "Process Name: " << data.process_name << " "
            //               << "CPU Time: " << data.cpu_time << " "
            //               << "RAM Usage: " << data.ram_usage << " "
            //               << "Disk Usage: " << data.disk_usage << "\n";
            // }

           
            sys_properties properties = get_system_properties();
            sys_properties availed_properties = get_availed_system_properties();
             json_string = process_to_json(logs, properties, availed_properties);
            // Send to cloud
            long http_status = rest_service::post(r_entity.resources_post_url, json_string);
            std::cout << json_string << '\n';
            DEBUG("Rest Api status: ", std::to_string(http_status));
            if (http_status == 200L)
            {
                LOG("Sent to cloud successfully");
                http_status = rest_service::start(r_entity, "process"); // see later
            }
            if (http_status != 200L && db.save(logs,properties,availed_properties) != Audit::SUCCESS)
            {
                DEBUG("Failed to store system process details locally");
            }
        }
        return res;
    }

    int get_app_resource_details(config_table_type &config_table)
    {
        int result = Audit::SUCCESS;
        vector<process_data> logs;
        process_entity entity = parser.get_process_entity(config_table);
        r_entity = parser.get_rest_entity(config_table);
        if (!validate_process_entity(entity))
        {
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

    sys_properties get_system_properties()
    {
        return service->get_system_properties();
    }

    sys_properties get_availed_system_properties()
    {
        return service->get_availed_system_properties();
    }

private:
    // std::ostream &operator<<(std::ostream &os, const process_data &data)
    // {
    //     os << "Process ID: " << data.process_id << "\n"
    //        << "Process Name: " << data.process_name << "\n"
    //        << "CPU Time: " << data.cpu_time << "\n"
    //        << "RAM Usage: " << data.ram_usage << "\n"
    //        << "Disk Usage: " << data.disk_usage << "\n";
    //     return os;
    // }
    int handle_process_details(const vector<process_data> &logs)
    {
        return Audit::SUCCESS;
    }

    bool validate_process_entity(process_entity &entity)
    {
        if (!entity.write_path.empty() && !os::is_dir_exist(entity.write_path))
        {
            LOG_ERROR("configured write directory path not exist " + entity.write_path);
            // common::write_log("proxy: validate_process_entity: configured write directory path not exist", Audit::FAILED);
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
    config_table_type config_table;
    config config_s;
    rest_entity r_entity;
    process_repository db;
};
#endif // !MONITOR_PROXXY_HPP