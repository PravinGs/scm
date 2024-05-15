#ifndef LOG_REPosITORY_HPP
#define LOG_REPosITORY_HPP

#pragma once

#include "LogEntity.hpp"

struct standardLogAttrs;

using namespace SCM;
using namespace SCM::Config;

class LogRepository
{
private:
    string getJsonWritePath(const string &name)
    {
        return os::createJSONFile(name);
    }

    int saveSysLogReport(LogEntity &entity, const vector<string> &logs, Json::Value &json)
    {
        string json_write_path = getJsonWritePath(entity.name);
        if (json_write_path.empty())
        {
            return FAILED;
        }
        DEBUG(json_write_path);
        fstream file(json_write_path, std::ios::out);
        Json::StreamWriterBuilder writer_builder;
        if (!file)
        {
            LOG_ERROR("LogRepository: saveSysLogReport: " + FWRITE_FAILED + json_write_path);
            return FAILED;
        }

        json["LogObjects"] = Json::Value(Json::arrayValue);
        for (auto log : logs)
        {
            Json::Value jsonLog;

            standardLogAttrs f_log = standardLogAttrs(log);
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
        DEBUG("LogRepository: saveSysLogReport: " + FWRITE_SUCCESS + json_write_path);
        return SUCCESS;
    }

    int saveAsJSON(LogEntity &entity, const vector<string> logs)
    {
        Json::Value json;
        if (entity.format == "syslog")
        {
            json["OrgId"] = SCM::Rest::ORG_ID;
            json["AppName"] = entity.name;
            json["Source"] = os::host_name;
            return saveSysLogReport(entity, logs, json);
        }
        else
        {
            std::cout << "Currently this format not supported" << '\n';
            return SUCCESS;
        }
    }

    int saveAsArchive(LogEntity &entity, const vector<string> &logs)
    {
        string file_path;
        auto today = std::chrono::system_clock::now();
        auto time_info = std::chrono::system_clock::to_time_t(today);
        std::tm *tm_info = std::localtime(&time_info);

        int day = tm_info->tm_mday;
        int month = tm_info->tm_mon + 1;
        int year = tm_info->tm_year + 1900;

        if (os::handleLocalLogFile(day, month, year, file_path, entity.name) == FAILED)
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
    int save(LogEntity &entity, const vector<string> &logs)
    {
        DEBUG("storing " + entity.name + " logs started");
        if (entity.storage_type == "json")
        {
            return saveAsJSON(entity, logs);
        }
        else
        {
            return saveAsArchive(entity, logs);
        }
    }
};

#endif //LOG_REPosITORY_HPP