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

struct sys_properties
{
    double ram;
    double disk;
    double cpu;
    sys_properties() : ram(0.0f), disk(0.0f), cpu(0.0f) {}
    sys_properties(double ram, double disk, double cpu)
        : ram(ram), disk(disk), cpu(cpu) {}
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

struct signature_entity
{
    string file_name;
    string signature;

    signature_entity(const string &file_name, const string &signature) : file_name(file_name), signature(signature) {}
};

static string process_to_json(const vector<process_data> &logs, const sys_properties &properties, const sys_properties &availed_properties);

string process_to_json(const vector<process_data> &logs, const sys_properties &properties, const sys_properties &availed_properties)
{
    Json::Value json;
    Json::Value props;
    props["CpuMemory"] = properties.cpu;
    props["RamMeomry"] = properties.ram;
    props["DiskMemory"] = properties.disk;
    Json::Value availed_props;
    availed_props["CpuMemory"] = availed_properties.cpu;
    availed_props["RamMeomry"] = availed_properties.ram;
    availed_props["DiskMemory"] = availed_properties.disk;

    json["DeviceTotalSpace"] = props;
    json["DeviceUsedSpace"] = availed_props;
    json["TimeGenerated"] = os::get_current_time();
    json["Source"] = os::host_name;
    json["OrgId"] = 10;
    json["ProcessObjects"] = Json::Value(Json::arrayValue);
    for (auto log : logs)
    {
        Json::Value json_log;
        process_data data = process_data(log);
        json_log["process_id"] = std::stoi(data.process_id);
        json_log["process_name"] = data.process_name;
        json_log["cpu_usage"] = std::stod(data.cpu_time);
        json_log["ram_usage"] = std::stod(data.ram_usage);
        json_log["disk_usage"] = std::stod(data.disk_usage);
        json["ProcessObjects"].append(json_log);
    }
    // json["status"] = "success";
    return json.toStyledString();
}
#endif // !MONITOR_ENTITY_HPP