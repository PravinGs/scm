#ifndef LOG_PROXY_HPP
#define LOG_PROXY_HPP
#pragma once

#include "util/config.hpp"
#include "log_collector/log_service_impl.hpp"
#include "util/entity_parser.hpp"

class log_proxy : public log_service
{
public:
    log_proxy() : thread_handler(true), service(std::make_unique<log_service_impl>()) {}
    ~log_proxy()
    {
        // for (auto &async_task : async_syslog_tasks)
        //         {
        // async_task.get();
        // }
    }
    int get_syslog(config_table_type &config_table)
    {
        int result = Audit::SUCCESS;
        log_entity entity = parser.get_log_entity(config_table, "syslog");
        std::vector<std::string> logs;

        if (entity.time_pattern.empty())
        {
            result = create_syslog_async_tasks(entity, logs);
        }
        else
        {
            schedular = cron::make_cron(entity.time_pattern);
            while (thread_handler)
            {
                common::pause_till_next_execution(schedular);
                result = create_syslog_async_tasks(entity, logs);
                if (result == Audit::FAILED)
                    thread_handler = false;
                common::write_log("log_controller: syslog_manager: thread execution done.", Audit::DEBUG);
            }
        }
        return result;
    }

    int get_syslog(log_entity &entity, vector<string> &logs)
    {
        if (!get_previous_log_read_time(entity) || !validate_log_entity(entity))
        {
            return Audit::FAILED;
        }
        DEBUG("reading " + entity.name);
        int result = service->get_syslog(entity, logs);
        if (result == Audit::SUCCESS && common::update_log_written_time(entity.name, entity.current_read_time) == Audit::SUCCESS)
        {
            LOG("LOG collected and cache updated");
        }
        return result;
    }

    int get_applog() { return Audit::SUCCESS; }

private: // member functions
    bool validate_log_entity(log_entity &entity)
    {
        try
        {
            if (!entity.columns.empty())
            {
                entity.json_attributes = config_s.to_vector(entity.columns, ',');
            }
            if (entity.last_read_time == std::numeric_limits<time_t>::min())
            {
                throw std::invalid_argument("proxy: validate_log_entity: no Specific time mentioned to collect log");
            }
            if (entity.format == "applog" && entity.columns.size() == 0)
            {
                throw std::invalid_argument("proxy: validate_log_entity: log attributes not configured for " + entity.name);
            }
            return true;
        }
        catch (exception &e)
        {
            string error = e.what();
            LOG_ERROR(error);
            // agent_utils::write_log("proxy: validate_log_entity: " + error, FAILED);
        }
        return false;
    }

    bool get_previous_log_read_time(log_entity &entity)
    {
        /*if (entity.last_read_time) {
            LOG("Last read time alredy defined");
            return true;
        }*/
        string file_path = os::get_path_or_backup_file_path(entity.read_path);
        if (file_path.size() == 0)
        {
            LOG_ERROR(Audit::INVALID_FILE, entity.read_path);
            // agent_utils::write_log("proxy: get_previous_log_read_time: invalid file " + entity.read_path, Audit::FAILED);
            return false;
        }
        string temp_config_path = Audit::Config::BASE_CONFIG_DIR;
        temp_config_path += Audit::Config::BASE_CONFIG_TMP + entity.name;
        fstream file(temp_config_path, std::ios::in);
        string last_time = "";
        if (file.is_open())
        {
            std::getline(file, last_time);
            file.close();
            if (last_time.size() == 19)
            {
                entity.last_read_time = common::string_to_time_t(last_time);
                return true;
            }
        }
        else
        {
            DEBUG("log reading directory not exists, creating new directory");
            // agent_utils::write_log("proxy: get_previous_log_read_time: log reading directory not exists, creating new directory");
            string tmp_config_dir = Audit::Config::BASE_CONFIG_DIR;
            if (!os::is_dir_exist(tmp_config_dir) && os::create_dir(tmp_config_dir) == Audit::FAILED)
            {
                return false;
            }
            tmp_config_dir += Audit::Config::BASE_CONFIG_TMP;
            if (!os::is_dir_exist(tmp_config_dir) && os::create_dir(tmp_config_dir) == Audit::FAILED)
            {
                return false;
            }
        }
        std::ofstream temp_config_file(temp_config_path);
        if (!temp_config_file)
        {
            LOG_ERROR(Audit::FILE_ERROR + temp_config_path);
            // agent_utils::write_log("proxy: get_previous_log_read_time: failed to create file check it's permission " + temp_config_path, Audit::FAILED);
            return false;
        }
        fstream fp(file_path, std::ios::in | std::ios::binary);
        if (!fp)
        {
            LOG_ERROR(Audit::FILE_ERROR, file_path);
            // agent_utils::write_log("proxy: get_previous_log_read_time: invalid Log file " + file_path, Audit::FAILED);
            temp_config_file.close();
            return false;
        }
        string line;
        std::getline(fp, line);
        if (entity.name == "syslog" || entity.name == "auth")
        {
            string timestamp = line.substr(0, 15);
            common::convert_syslog_to_utc_format(timestamp, last_time);
        }
        else if (entity.name == "dpkg")
        {
            last_time = line.substr(0, 19);
        }
        temp_config_file << last_time;
        fp.close();
        temp_config_file.close();
        entity.last_read_time = common::string_to_time_t(last_time);
        return true;
    }

    int create_syslog_async_tasks(log_entity &entity, vector<string> &logs)
    {
        int result;
        vector<string> syslog_files = config_s.to_vector(entity.read_path, ',');
        for (const string &file : syslog_files)
        {
            string app_name;
            if (file.size() == 0)
                continue;

            size_t start = file.find_last_of('/');
            size_t end = file.find_first_of('.');

            if (start != std::string::npos && end != std::string::npos)
            {
                app_name = file.substr(start + 1, end - start - 1);
            }
            else if (start != std::string::npos && end == std::string::npos)
            {
                app_name = file.substr(start + 1);
            }
            else
            {
                break;
            }
            entity.read_path = file;
            entity.name = app_name;
            result = get_syslog(const_cast<log_entity &>(entity), logs);
            // auto async_task = [&, entity]() -> int
            //             {
            // std::lock_guard<std::mutex> lock(logs_mutex);
            // return get_syslog(const_cast<log_entity &>(entity), logs);
            // };
            // async_syslog_tasks.push_back(std::async(std::launch::async, async_task));
        }
        if (logs.size() > 0)
        {
            for (const string &log : logs)
            {
                std::cout << log << '\n';
            }
        }
        /*
         * do the connection work here.
         */

        return result;
    }

private:
    bool thread_handler;
    std::unique_ptr<log_service> service;
    config config_s;
    entity_parser parser;
    // vector<std::future<int>> async_syslog_tasks;
    cron::cronexpr schedular;
};

#endif // !LOG_PROXY_HPP