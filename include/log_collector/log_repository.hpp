#ifndef LOG_REPOSITORY_HPP
#define LOG_REPOSITORY_HPP

#pragma once

#include "log_entity.hpp"

struct standard_log_attrs;

using namespace Audit;
using namespace Audit::Config;

class log_repository
{
private:
    string get_json_write_path(const string &timestamp, const string &name)
    {
        string file_path = BASE_LOG_DIR;

        if (!os::is_dir_exist(file_path))
        {
            os::create_dir(file_path);
        }

        file_path += "json/";

        if (!os::is_dir_exist(file_path))
        {
            os::create_dir(file_path);
        }

        file_path += name;

        if (!os::is_dir_exist(file_path))
        {
            os::create_dir(file_path);
        }

        file_path += "/" + timestamp + "-" + name + ".json";

        std::ofstream file(file_path);
        if (!file)
        {
            LOG_ERROR(FILE_ERROR, file_path);
            return "";
        }
        file.close();
        return file_path;
    }

    int save_syslog(log_entity &entity, const vector<string> &logs, Json::Value &json)
    {
        string json_write_path = get_json_write_path(entity.current_read_time, entity.name);
        if (json_write_path.empty())
        {
            return FAILED;
        }
        DEBUG(json_write_path);
        fstream file(json_write_path, std::ios::out);
        Json::StreamWriterBuilder writer_builder;
        if (!file)
        {
            LOG_ERROR("log_repository: save_syslog: " + FWRITE_FAILED + json_write_path);
            return FAILED;
        }

        json["LogObjects"] = Json::Value(Json::arrayValue);
        for (auto log : logs)
        {
            Json::Value jsonLog;

            standard_log_attrs f_log = standard_log_attrs(log);
            jsonLog["TimeGenerated"] = f_log.timestamp;
            jsonLog["UserLoginId"] = f_log.user;
            jsonLog["ServiceName"] = f_log.program;
            jsonLog["Message"] = f_log.message;
            jsonLog["LogLevel"] = f_log.level;
            jsonLog["LogCategory"] = f_log.category;

            json["LogObjects"].append(jsonLog);
        }

        std::unique_ptr<Json::StreamWriter> writer(writer_builder.newStreamWriter());
        writer->write(json, &file);
        file.close();
        DEBUG("log_repository: save_syslog: " + FWRITE_SUCCESS + json_write_path);
        return SUCCESS;
    }

    int save_as_json(log_entity &entity, const vector<string> logs)
    {
        Json::Value json;
        if (entity.format == "syslog")
        {
            json["OrgId"] = Audit::Rest::ORG_ID;
            json["AppName"] = entity.name;
            json["Source"] = os::host_name;
            return save_syslog(entity, logs, json);
        }
        else
        {
            std::cout << "Currently this format not supported" << '\n';
            return SUCCESS;
        }
    }

    int save_as_archive(log_entity &entity, const vector<string> &logs)
    {
        string file_path;
        auto today = std::chrono::system_clock::now();
        auto time_info = std::chrono::system_clock::to_time_t(today);
        std::tm *tm_info = std::localtime(&time_info);

        int day = tm_info->tm_mday;
        int month = tm_info->tm_mon + 1;
        int year = tm_info->tm_year + 1900;

        if (os::handle_local_log_file(day, month, year, file_path, entity.name) == FAILED)
        {
            return FAILED;
        }

        fstream file(file_path, std::ios::app);

        for (string line : logs)
        {
            file << line << "\n";
        }
        file.close();

        DEBUG(FWRITE_SUCCESS, file_path);

        return SUCCESS;
    }

public:
    int save(log_entity &entity, const vector<string> &logs)
    {
        DEBUG("log_repository: save: storing " + entity.name + " logs started");
        if (entity.storage_type == "json")
        {
            return save_as_json(entity, logs);
        }
        else
        {
            return save_as_archive(entity, logs);
        }
    }
};

#endif