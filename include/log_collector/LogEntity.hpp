#ifndef LOG_ENTITY_HPP
#define LOG_ENTITY_HPP
#pragma once
#include "util/Common.hpp"

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
    std::time_t end_time;
    string current_read_time;
    conn_entity connection;   
    string err_msg;
    LogEntity() : count(0), is_empty(true), start_filter(false), end_filter(false), last_read_time(0), end_time(0) {}
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

#endif // !LOG_ENTITY_HPP