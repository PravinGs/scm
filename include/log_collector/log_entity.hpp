#ifndef LOG_ENTITY_HPP
#define LOG_ENTITY_HPP
#pragma once
#include "util/common.hpp"
struct log_entity
{
    int count;
    char remote = 'n';
    bool is_empty;
    char delimeter = ',';
    string format;
    string name;
    string read_path;
    string write_path;
    vector<string> json_attributes;
    string columns;
    string time_pattern;
    string storage_type;
    vector<string> log_levels;
    std::time_t last_read_time;
    string current_read_time;
    conn_entity connection;
    log_entity() : count(0), is_empty(true), last_read_time(std::time(nullptr)) {}
};
 
#endif // !LOG_ENTITY_HPP