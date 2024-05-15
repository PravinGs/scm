#ifndef MONITOR_SERVICE_IMPL_HPP
#define MONITOR_SERVICE_IMPL_HPP
#pragma once
#include "monitor/MonitorService.hpp"

using namespace SCM::Monitor;
using namespace SCM;

class MonitorServiceImpl : public MonitorService
{
public:
    ~MonitorServiceImpl()
    {
        // for (auto &task : async_tasks)
        // {
        //     task.get();
        // }
    }

    int getAppResourceDetails(const ProcessEntity &entity, vector<ProcessData> &logs)
    {
        DEBUG("request for collecting process details");
        vector<int> process_ids = getAllProcessId();
        for (const int process_id : process_ids)
        {
            // auto async_task = [&, process_id]()
            // {
            //     ProcessData p = createProcessData(process_id);
            //     std::lock_guard<std::mutex> lock(process_mutex);
            //     logs.push_back(p);
            // };
            // async_tasks.push_back(std::async(std::launch::async, async_task));
            ProcessData p = createProcessData(process_id);
            // std::lock_guard<std::mutex> lock(process_mutex);
            logs.push_back(p);
        }
        DEBUG("process information collected");
        return SCM::SUCCESS;
    }

    int getAppResourceDetails(config_table_type &config_table) { return SCM::SUCCESS; }

    SystemProperties getSystemProperties()
    {
        DEBUG("request for collecting system properties");
        SystemProperties properties;
        struct statvfs buffer;
        if (statvfs("/", &buffer) == 0)
        {
            unsigned long long totalSpace = buffer.f_blocks * buffer.f_frsize;
            properties.disk = static_cast<double>(totalSpace) / (1024 * 1024 * 1024);
        }
        std::ifstream meminfo(PROC + "meminfo");
        std::string line;
        unsigned long long total_memory = 0;
        unsigned long long free_memory = 0;
        while (std::getline(meminfo, line))
        {
            if (line.find("MemTotal:") != std::string::npos)
            {
                sscanf(line.c_str(), "MemTotal: %llu", &total_memory);
            }
            else if (line.find("MemFree:") != std::string::npos)
            {
                sscanf(line.c_str(), "MemFree: %llu", &free_memory);
            }
        }
        properties.ram = static_cast<double>(total_memory) / (1024 * 1024);
        meminfo.close();
        std::ifstream stat_file("/proc/stat");
        unsigned long long user_time = 0;
        unsigned long long nice_time = 0;
        unsigned long long system_time = 0;
        unsigned long long idleTime = 0;
        while (std::getline(stat_file, line))
        {
            if (line.find("cpu ") != std::string::npos)
            {
                sscanf(line.c_str(), "cpu %llu %llu %llu %llu", &user_time, &nice_time, &system_time, &idleTime);
                break;
            }
        }
        unsigned long long total_time = user_time + nice_time + system_time + idleTime;
        properties.cpu = static_cast<double>(total_time);
        stat_file.close();
        return properties;
    }

    SystemProperties getAvailedSystemProperties()
    {
        DEBUG("request for collecting availed system properties");
        SystemProperties properties;
        struct statvfs buffer;
        if (statvfs("/", &buffer) == 0)
        {
            unsigned long long totalSpace = buffer.f_blocks * buffer.f_frsize;
            unsigned long long availableSpace = buffer.f_bavail * buffer.f_frsize;
            unsigned long long usedSpace = totalSpace - availableSpace;
            properties.disk = static_cast<double>(usedSpace) / totalSpace * 100;
        }
        std::ifstream meminfo(PROC + "meminfo");
        std::string line;
        unsigned long long total_memory = 0;
        unsigned long long freeMemory = 0;
        while (std::getline(meminfo, line))
        {
            if (line.find("MemTotal:") != std::string::npos)
            {
                sscanf(line.c_str(), "MemTotal: %llu", &total_memory);
            }
            else if (line.find("MemFree:") != std::string::npos)
            {
                sscanf(line.c_str(), "MemFree: %llu", &freeMemory);
            }
        }
        unsigned long long usedMemory = total_memory - freeMemory;
        properties.ram = static_cast<double>(usedMemory) / total_memory * 100;
        meminfo.close();
        std::ifstream statFile("/proc/stat");
        unsigned long long userTime = 0;
        unsigned long long niceTime = 0;
        unsigned long long systemTime = 0;
        unsigned long long idleTime = 0;

        while (std::getline(statFile, line))
        {
            if (line.find("cpu ") != std::string::npos)
            {
                sscanf(line.c_str(), "cpu %llu %llu %llu %llu", &userTime, &niceTime, &systemTime, &idleTime);
                break;
            }
        }
        unsigned long long totalTime = userTime + niceTime + systemTime + idleTime;
        properties.cpu = (static_cast<double>(totalTime) - idleTime) / totalTime * 100;
        statFile.close();
        return properties;
    }

    // private:
    vector<int> getAllProcessId()
    {
        vector<int> process_ids;
#if __linux__
        int pid = -1;
        DIR *dir = opendir(SCM::Monitor::PROC.c_str());
        dirent *entry = nullptr;
        if (dir == nullptr)
        {
            DEBUG(INVALID_PATH, PROC);
            return process_ids;
        }
        while ((entry = readdir(dir)) != nullptr)
        {
            if (entry->d_type == DT_DIR)
            {
                try
                {
                    pid = std::stoi(entry->d_name);
                    process_ids.push_back(pid);
                }
                catch (const std::exception &e)
                {
                    DEBUG(e.what());
                }
            }
        }
#endif
        return process_ids;
    }

    double calculateCpuTime(CpuTable &table)
    {
        double utime = table.utime / CLK_TCK;
        double stime = table.stime / CLK_TCK;
        double start_time = table.start_time / CLK_TCK;
        double elapsed_time = table.get_up_time() - start_time;
        double cpu_run_time = ((utime + stime) * 100) / elapsed_time;
        cpu_run_time = cpu_run_time * (1 + (table.nice_time / MAX_NICE_VALUE));
        return cpu_run_time;
    }

    double getMemoryUsage(const unsigned int &process_id)
    {
        unsigned long size, resident, shared, text, lib, data, dt;
        double memory_percentage = -1.0;
        string line;
        string path = PROC + std::to_string(process_id) + MEMORYDATA;
        long page_size = sysconf(_SC_PAGESIZE);
        long total_memory = sysconf(_SC_PHYS_PAGES) * page_size;
        std::ifstream file(path);
        if (!file)
        {
            LOG_ERROR(FILE_ERROR, path);
            return memory_percentage;
        }
        std::getline(file, line);
        std::istringstream iss(line);
        if (!(iss >> size >> resident >> shared >> text >> lib >> data >> dt))
        {
            LOG_ERROR("failed to parse memory statistics.");
            return memory_percentage;
        }
        memory_percentage = 100.0 * resident * page_size / total_memory;
        file.close();
        return memory_percentage;
    }

    double getDiskUsage(const unsigned int &process_id)
    {
        double disk_usage = -1.0;
        int i = 0;
        string line;
        string path = PROC + std::to_string(process_id) + IO;
        std::fstream file(path, std::ios::in);
        if (!file.is_open())
        {
            LOG_ERROR("process does not exist with this id : " + std::to_string(process_id));
            return disk_usage;
        }
        disk_usage = 0.0;
        while (std::getline(file, line))
        {
            string token;
            std::stringstream ss(line);
            if (i == 4 || i == 5)
            {
                while (std::getline(ss, token, ':'))
                {
                    try
                    {
                        double d = std::stod(token);
                        disk_usage += d;
                    }
                    catch (std::exception &e)
                    {
                    }
                }
            }
            i++;
        }
        file.close();
        return disk_usage;
    }

    string getProcessNameById(const unsigned int &process_id)
    {
        if (process_id <= 0)
        {
            return "";
        }
        string process_name = "";
        string path = PROC + std::to_string(process_id) + COMM;
        std::ifstream file(path);
        if (file.is_open())
        {
            std::getline(file, process_name);
            file.close();
        }
        return process_name;
    }

    CpuTable getProcessTimeById(const unsigned int &process_id)
    {
        string path = SCM::Monitor::PROC + std::to_string(process_id) + SCM::Monitor::CPUDATA;
        vector<string> stats;
        string line, word;
        fstream file(path, std::ios::in);
        if (!(file.is_open()))
        {
            CpuTable emptyTable;
            DEBUG(FILE_ERROR + path);
            return emptyTable;
        }
        std::getline(file, line);
        std::stringstream ss(line);
        while (std::getline(ss, word, ' '))
        {
            stats.push_back(word);
        }
        file.close();
        CpuTable table(
            stoi(stats.at(UITME)), stoi(stats.at(STIME)),
            stoi(stats.at(CUTIME)), stoi(stats.at(CSTIME)),
            stoi(stats.at(NICETIME)), stoi(stats.at(START_TIME)));
        return table;
    }

    ProcessData createProcessData(const int process_id)
    {
        CpuTable table = getProcessTimeById(process_id);
        string process_name = getProcessNameById(process_id);
        double cpuTime = calculateCpuTime(table);
        double memUsage = getMemoryUsage(process_id);
        double disk_usage = getDiskUsage(process_id);
        ProcessData processData{
            std::to_string(process_id), process_name,
            std::to_string(cpuTime), std::to_string(memUsage),
            std::to_string(disk_usage)};
        return processData;
    }

    // private:
    // vector<std::future<void>> async_tasks;
    std::mutex process_mutex;
};
#endif // !MONITOR_SERVICE_IMPL_HPP