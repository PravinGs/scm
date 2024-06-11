#ifndef MQTT_REPORT_GENERATION_HPP
#define MQTT_REPORT_GENERATION_HPP

#include "util/Common.hpp"
#include "monitor/MonitorEntity.hpp"
#include "SignatureRepository.hpp"
#include "log_collector/LogEntity.hpp"

class MqttReport
{
public:
    int logReport(LogEntity &entity, const vector<string> &logs, Json::Value &json)
    {
        string json_write_path = os::createJSONFile("process");
        if (json_write_path.empty())
        {
            return SCM::FAILED;
        }
        DEBUG(json_write_path);
        fstream file(json_write_path, std::ios::out);
        Json::StreamWriterBuilder writer_builder;
        if (!file)
        {
            LOG_ERROR("MqttReportGeneration: logReport: " + SCM::FWRITE_FAILED + json_write_path);
            return SCM::FAILED;
        }

        json["LogObjects"] = Json::Value(Json::arrayValue);
        for (auto log : logs)
        {
            Json::Value jsonLog;

            standardLogAttrs f_log = standardLogAttrs(log);
            jsonLog["TimeGenerated"] = f_log.timestamp;
            jsonLog["UserLoginId"] = f_log.user;
            jsonLog["ServiceName"] = f_log.program;
            jsonLog["Message"] = f_log.message;
            jsonLog["LogLevel"] = f_log.level;
            jsonLog["LogCategory"] = f_log.category;

            json["LogObjects"].append(jsonLog);
        }

        std::unique_ptr<Json::StreamWriter> writer(writer_builder.newStreamWriter());
        writer->write(json, &file);
        file.close();
        DEBUG("MqttReportGeneration: logReport: " + SCM::FWRITE_SUCCESS + json_write_path);
        return SCM::SUCCESS;
    }

private:
    const string key = "pappadam";

public:
    int monitorReport(const vector<ProcessData> &logs, const SystemProperties &properties, const SystemProperties &availed_properties)
    {
        std::string path = os::createJSONFile("process");
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
            LOG_ERROR(SCM::FWRITE_FAILED + path);
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
        DEBUG(SCM::FWRITE_SUCCESS + path);
        return SCM::SUCCESS;
    }
};

#endif

int main()
{
    LogEntity logEntity{"system_logs"};

    const vector<string> &logs;
    const vector<ProcessData> &logs;

    SystemProperties properties{100.0, 2000.0, 500.0};
    SystemProperties availed_properties{50.0, 1000.0, 250.0};

    Json::Value jsonReport;
    jsonReport["timestamp"] = os::getCurrentTime();
    jsonReport["tpm_keys"] = getTpmKeys();

    int logResult = logReport(logEntity, logs, jsonReport);
    if (logResult == SCM::SUCCESS)
    {
        std::cout << "Logs saved successfully." << std::endl;
    }
    else
    {
        std::cout << "Failed to save logs." << std::endl;
    }

    int result = monitorReport(logs, properties, availed_properties);
    if (result == SCM::SUCCESS)
    {
        std::cout << "Process data saved successfully." << std::endl;
    }
    else
    {
        std::cout << "Failed to save process data." << std::endl;
    }

    return logResult == SCM::SUCCESS && result == SCM::SUCCESS ? SCM::SUCCESS : SCM::FAILED;
}
