#include "MqttStringBuilder.hpp"

template <>
string MqttStringBuilder::toJson(const MqttAck &response)
{
    Json::Value j;
    j["Status"] = response.status;
    j["Payload"] = response.payload;
    j["ErrorMessage"] = response.errMsg;
    return j.toStyledString();
}

template <>
string MqttStringBuilder::toJson(const MqttErrorReponse &response)
{
    Json::Value j;
    j["Status"] = response.status;
    j["Payload"] = response.payload;
    j["ErrorMessage"] = response.errMsg;
    return j.toStyledString();
}

template <>
string MqttStringBuilder::toJson(const MqttResponse &response)
{
    Json::Value j;
    j["Status"] = response.status;
    j["ErrorMessage"] = Common::getErrorString(response.status);
    j["RequestId"] = response.requestId;
    j["ActionType"] = response.actionType;
    j["Result"] = response.result;
    return j.toStyledString();
}

Json::Value MqttStringBuilder::toJson(const vector<std::string> &logs, const string &name)
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
    return json;
}

Json::Value MqttStringBuilder::toJson(const vector<ProcessData> &logs)
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
    return json;
}