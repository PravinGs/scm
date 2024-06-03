#ifndef MQTT_PARSER_HPP
#define MQTT_PARSER_HPP
#include "util/Common.hpp"
#include "log_collector/LogEntity.hpp"
#include "monitor/MonitorEntity.hpp"
#include "MqttEntity.hpp"

class mq
{
public:
    int extractRequestype(const std::string &json_string)
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
        return actionTypeToInt(root["ActionType"].asString());
    }

    int actionTypeToInt(const string &actionType)
    {
        if (actionType == "LogRequest")
        {
            return 0;
        }
        else if (actionType == "FirmwareUpdate")
        {
            return 1;
        }
        else if (actionType == "ProcessRequest")
        {
            return 2;
        }
        else if (actionType == "TpmConfigServiceuration")
        {
            return 3;
        }
        else
        {
            return -1;
        }
    }

    int responseTypeToInt(const string &responseType)
    {
        if (responseType == "MqttResponse")
        {
            return 0;
        }
        else if (responseType == "RestApiResponse")
        {
            return 1;
        }
        else
        {
            return -1;
        }
    }

    std::string actionValueToString(ActionType value)
    {
        switch (value)
        {
        case ActionType::LogRequest:
            return "LogRequest";
        case ActionType::PatchRequest:
            return "FirmwareUpdate";
        case ActionType::ProcessRequest:
            return "ProcessRequest";
        case ActionType::TpmConfigServiceuration:
            return "TpmConfigServiceuration";
        }
        return ""; // Handle default case
    }

    std::string tpmCommandToString(TpmCommand command)
    {

        switch (command)
        {
        case TpmCommand::TpmClear:
            return "TpmClear";
        case TpmCommand::SealKey:
            return "SealKey";
        case TpmCommand::UnsealKey:
            return "UnsealKey";
        case TpmCommand::NvStore:
            return "NvStore";
        case TpmCommand::NvRead:
            return "NvRead";
        }
        return "";
    }

    std::string responseTypeToString(int value)
    {
        if (value == 0)
        {
            return "MqttResponse";
        }
        else if (value == 1)
        {
            return "RestApiResponse";
        }
        else
        {
            return "";
        }
    }

    std::string logToJSON(const vector<std::string> &logs, const string &name)
    {
        Json::Value json;
        json["OrgId"] = 234225;
        json["AppName"] = name;
        json["Source"] = os::host_name;
        json["LogObjects"] = Json::Value(Json::arrayValue);
         json["TimeGenerated"] = os::getCurrentTime();
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
        json["status"] = "success";
        return json.toStyledString();
    }

    std::string ProcessToJSON(const vector<ProcessData> &logs)
    {
        DEBUG("Preparing Log response string.");
        Json::Value json;
        json["OrgId"] = 234225;
        json["AppName"] = "system_resources";
        json["Source"] = os::host_name;
        json["ProcessObjects"] = Json::Value(Json::arrayValue);
        json["TimeGenerated"] = os::getCurrentTime();
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
    LogRequest extractLogRequest(const string &json_string)
    {
        LogRequest request;
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

        request.startDate = root["FromDate"].asString();
        request.endDate = root["EndDate"].asString();
        request.actionType = actionTypeToInt(root["ActionType"].asString());
        request.id = root["Id"].asString();
        request.sourceId = root["SourceId"].asString();
        request.targetId = root["TargetId"].asString();
        request.isAckRequired = root["IsAckRequired"].asBool() ? 1 : 0;
        request.responseType = responseTypeToInt(root["ResponseType"].asString());
        request.logType = root["LogType"].asString();
        request.logLevels = Common::toVector(root["LogLevels"].asString(), ',');
        return request;
    }

    ProcessRequest extractProcessRequest(const string &json_string)
    {
        ProcessRequest request;
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
        request.actionType = actionTypeToInt(root["ActionType"].asString());
        request.sourceId = root["SourceId"].asString();
        request.isAckRequired = root["IsAckRequired"].asBool() ? 1 : 0;
        request.responseType = responseTypeToInt(root["ResponseType"].asString());
        request.process_names = Common::toVector(root["ProcessNames"].asString(),',');
        request.is_SystemProperties_required = root["SystemProperties"].asInt();
        return request;
    }

    PatchRequest extractPatchRequest(string json_string)
    {
        PatchRequest request;
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
        request.actionType = actionTypeToInt(root["ActionType"].asString());
        request.sourceId = root["SourceId"].asString();
        request.isAckRequired = root["IsAckRequired"].asBool() ? 1 : 0;
        request.responseType = responseTypeToInt(root["ResponseType"].asString());

        request.distributionType = root["distribution_type"].asInt();
        request.url = root["url"].asString();
        request.patchVersion = root["patch_version"].asString();
        request.username = root["username"].asString();
        request.password = root["password"].asString();
        return request;
    }

    TpmRequest extractTpmRequest(const std::string &json_string)
    {
        TpmRequest ConfigService;
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::istringstream iss(json_string);
        std::string errs;

        Json::parseFromStream(builder, iss, &root, &errs);

        if (!errs.empty())
        {
            std::cerr << "Error: Failed to parse JSON: " << errs << "\n";
            return ConfigService;
        }

        ConfigService.actionType = actionTypeToInt(root["ActionType"].asString());
        ConfigService.sourceId = root["SourceId"].asString();
        ConfigService.isAckRequired = root["IsAckRequired"].asBool() ? 1 : 0;
        ConfigService.responseType = responseTypeToInt(root["ResponseType"].asString());
        ConfigService.command = root["command"].asInt();

        // switch (static_cast<TpmCommand>(ConfigService.command))
        // {
        // case TpmCommand::TpmClear:
        // {
        //     string lockout_auth = root["lockout_auth"].asString();
        //     TpmConfig tpmConfigService(ConfigService.source_id, ConfigService.ActionType, ConfigService.is_ack_required, ConfigService.ResponseType, ConfigService.command, lockout_auth);
        //     return TpmRequest(tpmConfigService); // Return as TpmRequest
        // }
        // default:
        //     break;
        // }

        return ConfigService;
    }

    TpmConfig getTpmConfig(const TpmRequest &request, const string &json_string)
    {
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::istringstream iss(json_string);
        std::string errs;

        Json::parseFromStream(builder, iss, &root, &errs);
        string lockout_auth = root["lockout_auth"].asString();
        TpmConfig ConfigService(request.sourceId, request.actionType, request.isAckRequired, request.responseType, request.command, lockout_auth);
        return ConfigService; // Return as TpmRequest
    }

    SealConfig getSealConfig(TpmRequest &request, const string &json_string)
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

        SealConfig ConfigService(request.sourceId, request.actionType, request.isAckRequired, request.responseType, request.command, ownerAuth, srkAuth, dekAuth, fileName, objectName);
        return ConfigService; // Return as TpmRequest
    }

    PersistConfig getTpmPersistContext(TpmRequest &request, const string &json_string)
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

        PersistConfig ConfigService(
            request.sourceId,
            request.actionType,
            request.isAckRequired,
            request.responseType,
            request.command,
            indexName,
            ownerPassword,
            fileName,
            indexValue,
            indexPassword,
            dataSize);
        return ConfigService; // Return as TpmRequest
    }
};

#endif