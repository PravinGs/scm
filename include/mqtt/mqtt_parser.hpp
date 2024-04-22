#ifndef MQTT_PARSER_HPP
#define MQTT_PARSER_HPP
#include "util/common.hpp"
#include "log_collector/log_entity.hpp"
#include "monitor/monitor_entity.hpp"
#include "mqtt_entity.hpp"

class mq
{
public:
    int extract_request_type(const std::string &json_string)
    {
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::istringstream iss(json_string);
        std::string errs;

        Json::parseFromStream(builder, iss, &root, &errs);

        if (!errs.empty())
        {
            std::cerr << "Error: Failed to parse JSON: " << errs << "\n";
            return -1;
        }
        return root["action_type"].asInt();
    }

    std::string action_value_to_string(action_type value)
    {
        switch (value)
        {
        case action_type::LogRequest:
            return "LogRequest";
        case action_type::PatchRequest:
            return "FirmwareUpdate";
        case action_type::ProcessRequest:
            return "ProcessRequest";
        case action_type::TpmConfiguration:
            return "TpmConfiguration";
        }
        return ""; // Handle default case
    }

    std::string tpm_command_to_string(tpm_command command)
    {

        switch (command)
        {
        case tpm_command::TpmClear:
            return "TpmClear";
        case tpm_command::SealKey:
            return "SealKey";
        case tpm_command::UnsealKey:
            return "UnsealKey";
        case tpm_command::NvStore:
            return "NvStore";
        case tpm_command::NvRead:
            return "NvRead";
        }
        return "";
    }

    std::string response_type_string(int value)
    {
        switch (static_cast<response_type>(value))
        {
        case response_type::MqttResponse:
            return "MqttResponse";
        case response_type::RestApiResponse:
            return "RestApiResponse";
        }
        return ""; // Handle default case
    }

    std::string log_to_json(const vector<std::string> &logs, const string& name)
    {
        Json::Value json;
        json["OrgId"] = 234225;
        json["AppName"] = name;
        json["Source"] = os::host_name;
        json["LogObjects"] = Json::Value(Json::arrayValue);
        for (auto log : logs)
        {
            Json::Value jsonLog;

            standard_log_attrs f_log = standard_log_attrs(log);
            jsonLog["TimeGenerated"] = f_log.timestamp;
            jsonLog["UserLoginId"] = f_log.user;
            jsonLog["ServiceName"] = f_log.program;
            jsonLog["Message"] = f_log.message;
            jsonLog["LogLevel"] = f_log.level;
            jsonLog["LogCategory"] = f_log.category;

            json["LogObjects"].append(jsonLog);
        }
        json["status"] = "success";
        return json.toStyledString();
    }

    std::string process_to_json(const vector<process_data> &logs)
    {
        Json::Value json;
        json["OrgId"] = 234225;
        json["AppName"] = "system_resources";
        json["Source"] = os::host_name;
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
        json["status"] = "success";
        return json.toStyledString();
    }

    // std::string patch_to_json(const vector<download_props> &logs)
    // {
    //     Json::Value json;
    //     json["PatchObjects"] = Json::Value(Json::arrayValue);
    //     for (auto log : logs)
    //     {
    //         Json::Value jsonLog;

    //         download_props p_log = download_props(log);
    //         jsonLog["Url"] = p_log.url;
    //         jsonLog["Writepath"] = p_log.writePath;
    //         jsonLog["RootDirectory"] = p_log.rootDir;
    //         jsonLog["FileName"] = p_log.fileName;
    //         jsonLog["DownloadPath"] = p_log.downloadPath;

    //         json["PatchObjects"].append(jsonLog);
    //     }
    //     json["status"] = "success";
    //     return json.toStyledString();
    // }
    log_request extract_log_request(string json_string)
    {
        log_request request;
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::istringstream iss(json_string);
        std::string errs;

        Json::parseFromStream(builder, iss, &root, &errs);

        if (!errs.empty())
        {
            std::cerr << "Error: Failed to parse JSON: " << errs << "\n";
            return request;
        }

        request.action_type = root["action_type"].asInt();
        request.source_id = root["source_id"].asString(); // Response topic // targetId-Publish
        request.is_ack_required = root["is_ack_required"].asInt();
        request.response_type = root["response_type"].asInt();

        request.log_type = root["log_type"].asString();
        request.log_level = root["log_level"].asInt();
        request.start_time = root["start_time"].asString();
        request.end_time = root["end_time"].asString();
        return request;
    }

    process_request extract_process_request(const string &json_string)
    {
        process_request request;
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::istringstream iss(json_string);
        std::string errs;

        Json::parseFromStream(builder, iss, &root, &errs);

        if (!errs.empty())
        {
            std::cerr << "Error: Failed to parse JSON: " << errs << "\n";
            return request;
        }
        request.action_type = root["action_type"].asInt();
        request.source_id = root["source_id"].asString();
        request.is_ack_required = root["is_ack_required"].asInt();
        request.response_type = root["response_type"].asInt();

        request.process_name = root["process_name"].asString();
        request.is_sys_properties_required = root["system_properties"].asInt();
        return request;
    }

    patch_request extract_patch_request(string json_string)
    {
        patch_request request;
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::istringstream iss(json_string);
        std::string errs;

        Json::parseFromStream(builder, iss, &root, &errs);

        if (!errs.empty())
        {
            std::cerr << "Error: Failed to parse JSON: " << errs << "\n";
            return request;
        }
        request.action_type = root["action_type"].asInt();
        request.source_id = root["source_id"].asString();
        request.is_ack_required = root["is_ack_required"].asInt();
        request.response_type = root["response_type"].asInt();

        request.distribution_type = root["distribution_type"].asInt();
        request.url = root["url"].asString();
        request.patch_version = root["patch_version"].asString();
        request.username = root["username"].asString();
        request.password = root["password"].asString();
        return request;
    }

    tpm_request extract_tpm_request(const std::string &json_string)
    {
        tpm_request config;
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::istringstream iss(json_string);
        std::string errs;

        Json::parseFromStream(builder, iss, &root, &errs);

        if (!errs.empty())
        {
            std::cerr << "Error: Failed to parse JSON: " << errs << "\n";
            return config;
        }

        config.action_type = root["action_type"].asInt();
        config.source_id = root["source_id"].asString();
        config.is_ack_required = root["is_ack_required"].asInt();
        config.response_type = root["response_type"].asInt();
        config.command = root["command"].asInt();

        // switch (static_cast<tpm_command>(config.command))
        // {
        // case tpm_command::TpmClear:
        // {
        //     string lockout_auth = root["lockout_auth"].asString();
        //     tpm_config tpmConfig(config.source_id, config.action_type, config.is_ack_required, config.response_type, config.command, lockout_auth);
        //     return tpm_request(tpmConfig); // Return as tpm_request
        // }
        // default:
        //     break;
        // }

        return config;
    }

    tpm_config get_tpm_config(const tpm_request &request, const string &json_string)
    {
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::istringstream iss(json_string);
        std::string errs;

        Json::parseFromStream(builder, iss, &root, &errs);
        string lockout_auth = root["lockout_auth"].asString();
        tpm_config config(request.source_id, request.action_type, request.is_ack_required, request.response_type, request.command, lockout_auth);
        return config; // Return as tpm_request
    }

    seal_config get_seal_config(tpm_request &request, const string &json_string)
    {
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::istringstream iss(json_string);
        std::string errs;

        Json::parseFromStream(builder, iss, &root, &errs);
        string ownerAuth = root["ownerAuth"].asString();
        string srkAuth = root["srkAuth"].asString();
        string dekAuth = root["dekAuth"].asString();
        string fileName = root["fileName"].asString();
        string objectName = root["objectName"].asString();

        seal_config config(request.source_id, request.action_type, request.is_ack_required, request.response_type, request.command, ownerAuth, srkAuth, dekAuth, fileName, objectName);
        return config; // Return as tpm_request
    }

    persist_config get_persist_context(tpm_request &request, const string &json_string)
    {
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::istringstream iss(json_string);
        std::string errs;

        Json::parseFromStream(builder, iss, &root, &errs);
        string indexName = root["indexName"].asString();
        string ownerPassword = root["ownerPassword"].asString();
        string fileName = root["fileName"].asString();
        int indexValue = root["indexValue"].asInt();
        string indexPassword = root["indexPassword"].asString();
        int dataSize = root["dataSize"].asInt();

        persist_config config(
            request.source_id,
            request.action_type,
            request.is_ack_required,
            request.response_type,
            request.command,
            indexName,
            ownerPassword,
            fileName,
            indexValue,
            indexPassword,
            dataSize);
        return config; // Return as tpm_request
    }
};

#endif