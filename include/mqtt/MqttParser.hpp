#ifndef MQTT_PARSER_HPP
#define MQTT_PARSER_HPP
#include "util/Common.hpp"
#include "log_collector/LogEntity.hpp"
#include "monitor/MonitorEntity.hpp"
#include "MqttEntity.hpp"
#include "log_collector/LogEntity.hpp"

class MqttParser
{
public:
    string extractRequestype(const std::string &json_string)
    {
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::istringstream iss(json_string);
        std::string errs;

        Json::parseFromStream(builder, iss, &root, &errs);

        if (!errs.empty())
        {
            std::cerr << "Error: Failed to parse JSON: " << errs << "\n";
            return "";
        }
        return root["ActionType"].asString();
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

    LogRequest extractLogRequest(const string &json_string, int &status, string &error)
    {
        LogRequest request;
        Json::Value root;
        Json::Value child;
        Json::CharReaderBuilder builder;
        std::istringstream iss(json_string);

        Json::parseFromStream(builder, iss, &root, &error);

        if (!error.empty())
        {
            status = SCM::StatusCode::MQTT_JSON_REQUEST_PARSER_ERROR;
            return request;
        }
        request.id = root["Id"].asString();
        request.sourceId = root["SourceId"].asString();
        request.targetId = root["TargetId"].asString();
        request.actionType = root["ActionType"].asString();
        request.isAckRequired = root["IsAckRequired"].asBool() ? 1 : 0;
        request.responseType = responseTypeToInt(root["ResponseType"].asString());
        child = root["InputParams"];
        request.inputParams = child;
        request.logType = child["LogType"].asString();
        request.logLevels = Common::toVector(child["LogLevels"].asString(), ',');
        request.startDate = child["FromDate"].asString();
        request.endDate = child["EndDate"].asString();
        return request;
    }

    ProcessRequest extractProcessRequest(const string &json_string, int &status, string &error)
    {
        ProcessRequest request;
        Json::Value root;
        Json::Value child;
        Json::CharReaderBuilder builder;
        std::istringstream iss(json_string);

        Json::parseFromStream(builder, iss, &root, &error);

        if (!error.empty())
        {
            status = SCM::StatusCode::MQTT_JSON_REQUEST_PARSER_ERROR;
            return request;
        }
        request.id = root["Id"].asString();
        request.actionType = root["ActionType"].asString();
        request.sourceId = root["SourceId"].asString();
        request.isAckRequired = root["IsAckRequired"].asBool() ? 1 : 0;
        request.responseType = responseTypeToInt(root["ResponseType"].asString());
        request.inputParams = root["InputParams"];
        request.process_names = Common::toVector(request.inputParams["ProcessNames"].asString(), ',');
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
        request.actionType = root["ActionType"].asString();
        request.sourceId = root["SourceId"].asString();
        request.isAckRequired = root["IsAckRequired"].asBool() ? 1 : 0;
        request.responseType = responseTypeToInt(root["ResponseType"].asString());

        request.distributionType = root["distribution_type"].asInt();
        request.url = root["url"].asString();
        request.patchVersion = root["patch_version"].asString();
        request.username = root["username"].asString();
        request.password = root["password"].asString();

        //     try
        //     {
        //         string validateVersion = validateAndEnsureNewPatch (patchVersion, url , username , password);
        //         //cout << "patch version" << validateVersion << "is valid and ready for processing. "<< endl;
        //     }
        //     catch (const invalid_argument &e)
        //     {
        //         cerr << " Validation error: " << e.what() << endl;
        //     }

        return request;
    }

    TpmClearRequest extractTpmClearRequest(const string &json_string, int &status, string &error)
    {
        TpmClearRequest request;
        Json::Value root;
        Json::Value child;
        Json::CharReaderBuilder builder;
        std::istringstream iss(json_string);

        Json::parseFromStream(builder, iss, &root, &error);

        if (!error.empty())
        {
            status = SCM::StatusCode::MQTT_JSON_REQUEST_PARSER_ERROR;
            return request;
        }

        request.id = root["Id"].asString();
        request.sourceId = root["SourceId"].asString();
        request.targetId = root["TargetId"].asString();
        request.actionType = root["ActionType"].asString();
        request.isAckRequired = root["IsAckRequired"].asBool() ? 1 : 0;
        request.responseType = responseTypeToInt(root["ResponseType"].asString());
        child = root["InputParams"];
        request.inputParams = child;
        request.lockoutAuth = child["LockoutAuth"].asString();
        request.isBackupenabled = child["Backup"].asBool();
        return request;
    }

    TpmPostRequest extractTpmSealRequest(const string &json_string, int &status, string &error)
    {
        TpmPostRequest request;
        Json::Value root;
        Json::Value child;
        Json::CharReaderBuilder builder;
        std::istringstream iss(json_string);

        Json::parseFromStream(builder, iss, &root, &error);

        if (!error.empty())
        {
            status = SCM::StatusCode::MQTT_JSON_REQUEST_PARSER_ERROR;
            return request;
        }

        request.id = root["Id"].asString();
        request.sourceId = root["SourceId"].asString();
        request.targetId = root["TargetId"].asString();
        request.actionType = root["ActionType"].asString();
        request.isAckRequired = root["IsAckRequired"].asBool() ? 1 : 0;
        request.responseType = responseTypeToInt(root["ResponseType"].asString());
        child = root["InputParams"];
        request.inputParams = child;
        request.dataSize = child["DataSize"].asInt();
        request.ownerAuth = child["OwnerAuth"].asString();
        request.srkAuth = child["SrkAuth"].asString();
        request.dekAuth = child["DekAuth"].asString();
        request.data = child["Data"].asString();
        request.objectName = child["ObjectName"].asString();
        return request;
    }
};

#endif