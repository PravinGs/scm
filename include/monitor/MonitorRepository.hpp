#ifndef MONITOR_REPosITORY_HPP
#define MONITOR_REPosITORY_HPP

#include "util/Common.hpp"
#include "MonitorEntity.hpp"
#include "SignatureRepository.hpp"

using namespace SCM;
using namespace SCM::Config;

class MonitorRepository
{
private:
    FileSecurity security;
    const string key = "pappadam";

private:
    int getJsonWritePath(string &timestamp)
    {
        string file_path = BASE_LOG_DIR;

        if (!os::isDirExist(file_path))
        {
            os::createDir(file_path);
        }

        file_path += "json";

        if (!os::isDirExist(file_path))
        {
            os::createDir(file_path);
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
    int save(const vector<ProcessData> &logs, const SystemProperties &properties, const SystemProperties &availed_properties)
    {
        string path = os::createJSONFile("process");
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
            return SCM::FAILED;
        }
        jsonData["DeviceTotalSpace"] = props;
        jsonData["DeviceUsedSpace"] = availed_props;
        jsonData["TimeGenerated"] = os::getCurrentTime();
        jsonData["Source"] = os::host_name;
        jsonData["OrgId"] = 10;
        jsonData["ProcessObjects"] = Json::Value(Json::arrayValue);
        for (ProcessData data : logs)
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
        security.signAndStoreSignature(path, key);
        std::this_thread::sleep_for(std::chrono::seconds(3));
        if (!security.verifySignature(path, key))
        {
            DEBUG("verifySignature: failed");
        }
        DEBUG(FWRITE_SUCCESS + path);
        return SCM::SUCCESS;
    }
};

#endif //MONITOR_REPosITORY_HPP