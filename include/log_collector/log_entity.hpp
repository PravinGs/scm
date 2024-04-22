#ifndef LOG_ENTITY_HPP
#define LOG_ENTITY_HPP
#pragma once
#include "util/common.hpp"

static string log_to_json(const vector<std::string> &logs, const string& app_name);
struct log_entity
{
    int count;
    char remote = 'n'; //
    bool is_empty;     //
    char delimeter = ',';
    string format; //
    string name;   //
    string read_path;
    string write_path;
    vector<string> json_attributes; // No Columns configured , default columns.
    string time_pattern;
    string storage_type; // dafult json
    vector<string> log_levels;
    std::time_t last_read_time;
    string current_read_time;
    conn_entity connection;
    log_entity() : count(0), is_empty(true), last_read_time(std::time(nullptr)) {}
};

typedef struct standard_log_attrs standard_log_attrs;

static map<string, int> LogCategory{{"sys", 2}, {"network", 3}, {"ufw", 4}};

struct standard_log_attrs
{
    string timestamp;
    string user;
    string program;
    string message;
    int level;
    int category;

    standard_log_attrs(const string &log)
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
        // common::convert_syslog_to_utc_format(timestamp, utc_time);
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

    ~standard_log_attrs() {}
};

string log_to_json(const vector<std::string> &logs, const string& app_name)
{
    Json::Value json;
    json["AppName"] = app_name;
    json["TimeGenerated"] = os::get_current_time();
    json["Source"] = os::host_name;
    json["OrgId"] = 10;
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
    // json["status"] = "success";
    return json.toStyledString();
}
#endif // !LOG_ENTITY_HPP