#ifndef MONITOR_SERVICE_IMPL_HPP
#define MONITOR_SERVICE_IMPL_HPP
#pragma once
#include "monitor/monitor_service.hpp"

using namespace Audit::Monitor;
using namespace Audit;

class monitor_service_impl : public monitor_service
{
public:
    ~monitor_service_impl()
    {
        // for (auto &task : async_tasks)
        // {
        //     task.get();
        // }
    }

    int get_app_resource_details(const process_entity &entity, vector<process_data> &logs)
    {
        DEBUG("request for collecting process details");
        vector<int> process_ids = get_all_process_id();
        for (const int process_id : process_ids)
        {
            // auto async_task = [&, process_id]()
            // {
            //     process_data p = create_process_data(process_id);
            //     std::lock_guard<std::mutex> lock(process_mutex);
            //     logs.push_back(p);
            // };
            // async_tasks.push_back(std::async(std::launch::async, async_task));
            process_data p = create_process_data(process_id);
            // std::lock_guard<std::mutex> lock(process_mutex);
            logs.push_back(p);
        }
        DEBUG("process information collected");
        return Audit::SUCCESS;
    }

    int get_app_resource_details(config_table_type &config_table) { return Audit::SUCCESS; }

    sys_properties get_system_properties()
    {
        DEBUG("request for collecting system properties");
        sys_properties properties;
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

    sys_properties get_availed_system_properties()
    {
        DEBUG("request for collecting availed system properties");
        // agent_utils::write_log("monitor_service: get_availed_system_properties: request for collecting availed system properties started..", INFO);
        sys_properties properties;
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
    vector<int> get_all_process_id()
    {
        vector<int> process_ids;
#if __linux__
        int pid = -1;
        DIR *dir = opendir(Audit::Monitor::PROC.c_str());
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

    double calculate_cpu_time(cpu_table &table)
    {
        double utime = table.utime / CLK_TCK;
        double stime = table.stime / CLK_TCK;
        double start_time = table.start_time / CLK_TCK;
        double elapsed_time = table.get_up_time() - start_time;
        double cpu_run_time = ((utime + stime) * 100) / elapsed_time;
        cpu_run_time = cpu_run_time * (1 + (table.nice_time / MAX_NICE_VALUE));
        return cpu_run_time;
    }

    double get_memory_usage(const unsigned int &process_id)
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

    double get_disk_usage(const unsigned int &process_id)
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

    string get_process_name_id(const unsigned int &process_id)
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

    cpu_table read_processing_time_id(const unsigned int &process_id)
    {
        string path = Audit::Monitor::PROC + std::to_string(process_id) + Audit::Monitor::CPUDATA;
        vector<string> stats;
        string line, word;
        fstream file(path, std::ios::in);
        if (!(file.is_open()))
        {
            cpu_table emptyTable;
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
        cpu_table table(
            stoi(stats.at(UITME)), stoi(stats.at(STIME)),
            stoi(stats.at(CUTIME)), stoi(stats.at(CSTIME)),
            stoi(stats.at(NICETIME)), stoi(stats.at(START_TIME)));
        return table;
    }

    process_data create_process_data(const int process_id)
    {
        cpu_table table = read_processing_time_id(process_id);
        string process_name = get_process_name_id(process_id);
        double cpuTime = calculate_cpu_time(table);
        double memUsage = get_memory_usage(process_id);
        double disk_usage = get_disk_usage(process_id);
        process_data processData{
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