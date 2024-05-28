#ifndef LOG_PROXY_HPP
#define LOG_PROXY_HPP
#pragma once

#include "util/ConfigService.hpp"
#include "LogServiceImpl.hpp"
#include "LogRepository.hpp"
#include "util/entity_parser.hpp"
#include "rest/RestService.hpp"

class LogProxy : public LogService
{
public:
    LogProxy(config_table_type &config_table) : config_table(config_table), thread_handler(true), service(std::make_unique<LogServiceImpl>())
    {
        r_entity = parser.getRestEntity(config_table);
    }

    ~LogProxy()
    {

    }

    int getSysLog()
    {
        int result = SCM::SUCCESS;
        LogEntity entity = parser.getLogEntity(config_table, "syslog");
        std::vector<std::string> logs;

        if (entity.time_pattern.empty())
        {
            result = createSysLogAsyncTasks(entity, logs);
        }
        else
        {
            schedular = cron::make_cron(entity.time_pattern);
            while (thread_handler)
            {
                Common::pauseTillNextExecution(schedular);
                result = createSysLogAsyncTasks(entity, logs);
                if (result == SCM::FAILED)
                    thread_handler = false;
                DEBUG("thread execution done");
            }
        }
        return result;
    }

    int getSysLog(LogEntity &entity, vector<string> &logs)
    {
        string json_string;
        if (entity.last_read_time == std::numeric_limits<time_t>::min() && !getPreviousLogReadTime(entity))
        {   
            return SCM::FAILED;
        }   
        if (!validateLogEntity(entity))
        {
            return SCM::FAILED;
        }
        DEBUG("reading " + entity.name + " log file.");
        int result = service->getSysLog(entity, logs);
        if (result == SCM::SUCCESS && 
                Common::updateLogWrittenTime(entity.name, entity.current_read_time) == SCM::SUCCESS && 
                    !logs.empty())
        {
            // json_string = logToJSON(logs, entity.name);

            // RestResponse response = RestService::http_post(r_entity.logs_post_url, json_string);
            // DEBUG("Rest Api status: ", std::to_string(response.http_code));
            // if (response.http_code != SCM::Rest::POST_SUCCESS && db.save(entity, logs) != SCM::SUCCESS)
            // {
            //     DEBUG("Failed to store ", entity.name, " data locally");
            // }
            // if (response.http_code == SCM::Rest::POST_SUCCESS)
            // {
            //     RestService::start(r_entity, entity.name);
            // }  
        }
        return result;
    }

    int getAppLog() { return SCM::SUCCESS; }

    // private: // member functions

    bool validateLogEntity(LogEntity &entity)
    {
        try
        {
            if (
                entity.read_path.empty() ||
                (entity.name == "applog" && entity.name == entity.format) ||
                (entity.format == "applog" && entity.json_attributes.size() == 0))
            {
                throw std::invalid_argument("read_path | format cannot be empty");
            }

            if (!entity.write_path.empty() && !os::isDirExist(entity.write_path)) // incorrect write path
            {
                throw std::invalid_argument("Invalid write path ConfigServiceured");
            }
            else
            {
                entity.write_path = SCM::Config::BASE_LOG_DIR;
            }

            if (!entity.time_pattern.empty() && !(validateCronExpression(entity.time_pattern)))
            {
                DEBUG("Invalid cron expression ignoring");
                entity.time_pattern = "";
            }

            if (entity.last_read_time == std::numeric_limits<time_t>::min())
            {
                throw std::invalid_argument("proxy: validateLogEntity: no Specific time mentioned to collect log");
            }

            if (entity.storage_type.empty() || entity.storage_type != "archive" || entity.storage_type != "json")
            {
                DEBUG("Ignoring invalid storage default json type assigned");
                entity.storage_type = "json";
            }
            return true;
        }
        catch (exception &e)
        {
            string error = e.what();
            LOG_ERROR(error);
        }
        return false;
    }

    bool validateCronExpression(const std::string &cron_expression)
    {
        std::regex pattern("^(\\*|([0-5]?[0-9])|([0-5]?[0-9]-[0-5]?[0-9])|([0-5]?[0-9],[0-5]?[0-9](,[0-5]?[0-9])*)|((\\*/)?[0-5]?[0-9](/[0-5]?[0-9])?))\\s+(\\*|([01]?[0-9]|2[0-3])|([01]?[0-9]|2[0-3])-([01]?[0-9]|2[0-3])|([01]?[0-9]|2[0-3],[01]?[0-9]|2[0-3](,[01]?[0-9]|2[0-3])*)|((\\*/)?([01]?[0-9]|2[0-3])(/[01]?[0-9]|2[0-3])?))\\s+(\\*|([01-2]?[0-9]|3[01])|([01-2]?[0-9]|3[01])-([01-2]?[0-9]|3[01])|([01-2]?[0-9]|3[01],[01-2]?[0-9]|3[01](,[01-2]?[0-9]|3[01])*)|((\\*/)?([01-2]?[0-9]|3[01])(/[01-2]?[0-9]|3[01])?))\\s+(\\*|([1-9]|1[012])|([1-9]|1[012])-([1-9]|1[012])|([1-9]|1[012],[1-9]|1[012](,[1-9]|1[012])*)|((\\*/)?([1-9]|1[012])(/[1-9]|1[012])?))\\s+(\\*|([0-7])|([0-7])-([0-7])|([0-7],[0-7](,[0-7])*)|((\\*/)?[0-7](/[0-7])?))$");
        return std::regex_match(cron_expression, pattern);
    }

    bool getPreviousLogReadTime(LogEntity &entity)
    {
        string file_path = os::getPathOrBackupFilePath(entity.read_path);
        if (file_path.size() == 0)
        {
            LOG_ERROR(SCM::INVALID_FILE, entity.read_path);
            return false;
        }
        string temp_ConfigService_path = SCM::Config::BASE_CONFIG_DIR;
        temp_ConfigService_path += SCM::Config::BASE_CONFIG_TMP + entity.name;
        fstream file(temp_ConfigService_path, std::ios::in);
        string last_time = "";
        if (file.is_open())
        {
            std::getline(file, last_time);
            file.close();
            if (last_time.size() == 19)
            {
                entity.last_read_time = Common::stringToTime(last_time);
                return true;
            }
        }
        else
        {
            DEBUG("log reading directory not exists, creating new directory");
            string tmp_ConfigService_dir = SCM::Config::BASE_CONFIG_DIR;
            if (!os::isDirExist(tmp_ConfigService_dir) && os::createDir(tmp_ConfigService_dir) == SCM::FAILED)
            {
                return false;
            }
            tmp_ConfigService_dir += SCM::Config::BASE_CONFIG_TMP;
            if (!os::isDirExist(tmp_ConfigService_dir) && os::createDir(tmp_ConfigService_dir) == SCM::FAILED)
            {
                return false;
            }
        }
        std::ofstream temp_config_file(temp_ConfigService_path);
        if (!temp_config_file)
        {
            LOG_ERROR(SCM::FILE_ERROR + temp_ConfigService_path);
            return false;
        }
        fstream fp(file_path, std::ios::in | std::ios::binary);
        if (!fp)
        {
            LOG_ERROR(SCM::FILE_ERROR, file_path);
            temp_config_file.close();
            return false;
        }
        string line;
        std::getline(fp, line);
        if (entity.name == "syslog" || entity.name == "auth")
        {
            string timestamp = line.substr(0, 15);
            Common::convertToUtc(timestamp, last_time);
        }
        else if (entity.name == "dpkg")
        {
            last_time = line.substr(0, 19);
        }
        temp_config_file << last_time;
        fp.close();
        temp_config_file.close();
        entity.last_read_time = Common::stringToTime(last_time);
        return true;
    }

    int createSysLogAsyncTasks(LogEntity &entity, vector<string> &logs)
    {
        int result;
        vector<string> syslog_files = configService.toVector(entity.read_path, ',');
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
            result = getSysLog(const_cast<LogEntity &>(entity), logs);
            // auto async_task = [&, entity]() -> int
            //             {
            // std::lock_guard<std::mutex> lock(logs_mutex);
            // return getSysLog(const_cast<LogEntity &>(entity), logs);
            // };
            // async_syslog_tasks.push_back(std::async(std::launch::async, async_task));
        }
        /*
         * do the connection work here.
         */

        return result;
    }

private:
    config_table_type config_table;
    bool thread_handler;
    std::unique_ptr<LogService> service;
    ConfigService configService;
    entity_parser parser;
    RestEntity r_entity;
    LogRepository db;
    // vector<std::future<int>> async_syslog_tasks;
    cron::cronexpr schedular;
};

#endif // !LOG_PROXY_HPP