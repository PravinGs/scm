#ifndef LOG_ENTITY_HPP
#define LOG_ENTITY_HPP
#pragma once
#include "util/Common.hpp"

static string logToJSON(const vector<std::string> &logs, const string& app_name);

struct LogEntity
{
    int count;
    char remote = 'n'; 
    bool is_empty;     
    bool start_filter;
    bool end_filter;
    char delimeter = ',';
    string format; 
    string name;   
    string read_path;
    string write_path;
    vector<string> json_attributes; // No Columns ConfigServiceured , default columns.
    string time_pattern;
    string storage_type; // dafult json
    vector<string> log_levels;
    std::time_t last_read_time;
    string current_read_time;
    conn_entity connection;
    LogEntity() : count(0), is_empty(true), start_filter(true), end_filter(true), last_read_time(std::time(nullptr)) {}
};

typedef struct standardLogAttrs standardLogAttrs;

static map<string, int> LogCategory{{"sys", 2}, {"network", 3}, {"ufw", 4}};

struct standardLogAttrs
{
    string timestamp;
    string user;
    string program;
    string message;
    int level;
    int category;

    standardLogAttrs(const string &log)
    {
        string t_level, t_category;
        std::stringstream ss(log);
        std::getline(ss, timestamp, '|');
        std::getline(ss, user, '|');
        std::getline(ss, program, '|');
        std::getline(ss, message, '|');
        std::getline(ss, t_level, '|');
        std::getline(ss, t_category, '|');
        level = handle_exception(t_level);
        category = LogCategory[t_category];
    }

    int handle_exception(const string &level)
    {
        int r;
        try
        {
            r = std::stoi(level);
        }
        catch (const std::exception &e)
        {
            r = 0;
        }
        return r;
    }
    
    ~standardLogAttrs() {}
};

// TODO check for format conversion.
string logToJSON(const vector<std::string> &logs, const string& app_name)
{
    Json::Value json;
    json["AppName"] = app_name;
    json["TimeGenerated"] = os::getCurrentTime();
    json["Source"] = os::host_name;
    json["OrgId"] = SCM::Rest::ORG_ID;
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
    return json.toStyledString();
}

#endif // !LOG_ENTITY_HPP