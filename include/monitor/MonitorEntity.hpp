#ifndef MONITOR_ENTITY_HPP
#define MONITOR_ENTITY_HPP

#pragma once
#include "util/Common.hpp"

struct ProcessEntity
{
    int status;
    string write_path;
    string time_pattern;
    string storage_type;
    conn_entity connection;
    string err_msg;
    ProcessEntity(): status(0) {}
};

struct ProcessData
{
    string process_id;
    string process_name;
    string cpu_time;
    string ram_usage;
    string disk_usage;
    ProcessData(string id, string name, string c_time, string m_sage, string d_usage)
        : process_id(id), process_name(name), cpu_time(c_time), ram_usage(m_sage), disk_usage(d_usage)
    {
    }
};

struct SystemProperties
{
    double ram;
    double disk;
    double cpu;
    SystemProperties() : ram(0.0f), disk(0.0f), cpu(0.0f) {}
    SystemProperties(double ram, double disk, double cpu)
        : ram(ram), disk(disk), cpu(cpu) {}
};

struct CpuTable
{
    int utime;
    int stime;
    int cutime;
    int cstime;
    int start_time;
    int nice_time;
    int up_time;
    int cpu_count;

    CpuTable(int utime, int stime, int cutime, int cstime, int nice_time, int start_time)
        : utime(utime), stime(stime), cutime(cutime), cstime(cstime), start_time(start_time), nice_time(nice_time)
    {
        this->up_time = get_up_time();
        this->cpu_count = (int)sysconf(_SC_NPROCESSORS_ONLN);
    }

    CpuTable() {}

    int get_up_time()
    {
        string path = SCM::Monitor::PROC + SCM::Monitor::BOOTTIME;
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

struct SignatureEntity
{
    string file_name;
    string signature;

    SignatureEntity(const string &file_name, const string &signature) : file_name(file_name), signature(signature) {}
};

static string ProcessToJSON(const vector<ProcessData> &logs, const SystemProperties &properties, const SystemProperties &availed_properties);

string ProcessToJSON(const vector<ProcessData> &logs, const SystemProperties &properties, const SystemProperties &availed_properties)
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
    json["TimeGenerated"] = os::getCurrentTime();
    json["Source"] = os::host_name;
    json["OrgId"] = 10;
    json["ProcessObjects"] = Json::Value(Json::arrayValue);
    for (auto log : logs)
    {
        Json::Value json_log;
        ProcessData data = ProcessData(log);
        json_log["process_id"] = std::stoi(data.process_id);
        json_log["process_name"] = data.process_name;
        json_log["cpu_usage"] = std::stod(data.cpu_time);
        json_log["ram_usage"] = std::stod(data.ram_usage);
        json_log["disk_usage"] = std::stod(data.disk_usage);
        json["ProcessObjects"].append(json_log);
    }
    return json.toStyledString();
}
#endif // !MONITOR_ENTITY_HPP