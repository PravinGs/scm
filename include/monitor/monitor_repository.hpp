#ifndef PROCESS_REPOSITORY_HPP
#define PROCESS_REPOSITORY_HPP

#include "util/common.hpp"
#include "monitor_entity.hpp"
#include "signature_repository.hpp"

using namespace Audit;
using namespace Audit::Config;

class process_repository
{
private:
    file_security security;
    const string key = "pappadam";

private:
    int get_json_write_path(string &timestamp)
    {
        string file_path = BASE_LOG_DIR;

        if (!os::is_dir_exist(file_path))
        {
            os::create_dir(file_path);
        }

        file_path += "json";

        if (!os::is_dir_exist(file_path))
        {
            os::create_dir(file_path);
        }

        file_path += "/" + timestamp + ".json";

        std::ofstream file(file_path);
        if (!file)
        {
            DEBUG(FILE_ERROR + file_path);
            return FAILED;
        }
        file.close();
        timestamp = file_path;
        return SUCCESS;
    }

public:
    int save(const vector<process_data> &logs, const sys_properties &properties, const sys_properties &availed_properties)
    {
        string path = os::create_json_file("process");
        Json::Value props;
        props["CpuMemory"] = properties.cpu;
        props["RamMeomry"] = properties.ram;
        props["DiskMemory"] = properties.disk;
        Json::Value availed_props;
        availed_props["CpuMemory"] = availed_properties.cpu;
        availed_props["RamMeomry"] = availed_properties.ram;
        availed_props["DiskMemory"] = availed_properties.disk;
        fstream file(path, std::ios::app);
        Json::Value jsonData;
        Json::StreamWriterBuilder writerBuilder;
        if (!file)
        {
            LOG_ERROR(FWRITE_FAILED + path);
            return Audit::FAILED;
        }
        jsonData["DeviceTotalSpace"] = props;
        jsonData["DeviceUsedSpace"] = availed_props;
        jsonData["TimeGenerated"] = os::get_current_time();
        jsonData["Source"] = os::host_name;
        jsonData["OrgId"] = 10;
        jsonData["ProcessObjects"] = Json::Value(Json::arrayValue);
        for (process_data data : logs)
        {
            Json::Value json_log;
            json_log["process_id"] = std::stoi(data.process_id);
            json_log["process_name"] = data.process_name;
            json_log["cpu_usage"] = std::stod(data.cpu_time);
            json_log["ram_usage"] = std::stod(data.ram_usage);
            json_log["disk_usage"] = std::stod(data.disk_usage);
            jsonData["ProcessObjects"].append(json_log);
        }

        std::unique_ptr<Json::StreamWriter> writer(writerBuilder.newStreamWriter());
        writer->write(jsonData, &file);
        file.close();
        security.sign_and_store_signature(path, key);
        std::this_thread::sleep_for(std::chrono::seconds(3));
        if (!security.verify_signature(path, key))
        {
            DEBUG("verify_signature: failed");
        }
        DEBUG(FWRITE_SUCCESS + path);
        return Audit::SUCCESS;
    }
};

#endif