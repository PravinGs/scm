#ifndef MONITOR_ENTITY_HPP
#define MONITOR_ENTITY_HPP
#pragma once
#include "util/common.hpp"

struct process_entity
{
    string write_path;
    string time_pattern;
    string storage_type;
    conn_entity connection;
};

struct process_data
{
    string process_id;
    string process_name;
    string cpu_time;
    string ram_usage;
    string disk_usage;
    process_data(string id, string name, string c_time, string m_sage, string d_usage)
        : process_id(id), process_name(name), cpu_time(c_time), ram_usage(m_sage), disk_usage(d_usage)
    {
    }

};

struct cpu_table
{
    int utime;
    int stime;
    int cutime;
    int cstime;
    int start_time;
    int nice_time;
    int up_time;
    int cpu_count;

    cpu_table(int utime, int stime, int cutime, int cstime, int nice_time, int start_time)
        : utime(utime), stime(stime), cutime(cutime), cstime(cstime), start_time(start_time), nice_time(nice_time)
    {
        this->up_time = get_up_time();
        this->cpu_count = (int)sysconf(_SC_NPROCESSORS_ONLN);
    }

    cpu_table() {}

    int get_up_time()
    {
        string path = Audit::Monitor::PROC + Audit::Monitor::BOOTTIME;
        string line, word;
        fstream file(path, std::ios::in);
        if (file.is_open())
        {
            std::getline(file, line);
            std::stringstream ss(line);
            std::getline(ss, word, ' ');
            file.close();
        }
        return std::stoi(word);
    }
};

#endif // !MONITOR_ENTITY_HPP