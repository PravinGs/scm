#ifndef MqttEntity_HPP
#define MqttEntity_HPP

#include "util/Common.hpp"

static const std::map<string, int> actionCommand = {
    {"LOG", 0},
    {"PRO", 1},
    {"PAT", 2},
    {"TPM", 3},
};

static const std::map<string, int> tpmCommand = {
    {"TPMSETUP",0},
    {"TPMCLR", 1},
    {"TPMCNGPASS", 2},
    {"TPMSEAL", 3},
    {"TPMUNSEAL", 4},
    {"TPMDELSEAL", 5},
    {"TPMADDKEY", 6},
    {"TPMGETKEY", 7},
    {"TPMDELKEY", 8}
};

enum class ActionType
{
    LogRequest = 0,
    PatchRequest = 1,
    ProcessRequest = 2,
    TpmClear = 3,
    TpmSealKey = 4,
};

enum class TpmCommand
{
    TpmClear = 0,
    SealKey = 1,
    UnsealKey = 2,
    NvStore = 3,
    NvRead = 4
};

struct MqttEntity
{
    int port;
    int qos;
    bool is_secure;
    string client_id;
    string conn_string;
    vector<string> topics;
    string ca_cert_path;
    string client_cert_path;
    conn_entity connection;
    MqttEntity() : port(8000), qos(1), is_secure(true) {}
};

struct BaseRequest
{
    std::string id;
    std::string topic;
    std::string targetId;
    std::string actionType;
    int isAckRequired;
    int responseType;
    Json::Value inputParams;

    BaseRequest() : isAckRequired(0), responseType(-1) {}
    BaseRequest(const std::string &topic, const std::string &actionType, int isAckRequired, int responseType)
        : topic(topic), actionType(actionType), isAckRequired(isAckRequired), responseType(responseType) {}
};

struct PatchRequest : BaseRequest
{
    int distributionType;
    std::string url;
    std::string patchVersion;
    std::string username;
    std::string password;
    PatchRequest() : distributionType(0) {}
};

struct LogRequest : BaseRequest
{
    std::string logType;
    std::string startDate;
    std::string endDate;
    vector<string> logLevels;
};

struct ProcessRequest : public BaseRequest
{
    int process_id;
    vector<string> process_names;
    int is_SystemProperties_required;
    double ram_memory;
    double disk_memory;
    double cpu_time;

    ProcessRequest() : process_id(0), is_SystemProperties_required(0), ram_memory(0.0f), disk_memory(0.0f), cpu_time(0.0f)
    {
    }
};

struct TpmClearRequest : BaseRequest
{
    string lockoutAuth;
    bool isBackupenabled;
    TpmClearRequest() : isBackupenabled(false) {}
};

struct TpmChangePasswordRequest : BaseRequest
{
    int type;
    string oldAuth;
    string newAuth;
    TpmChangePasswordRequest() : type(0) {}
};

struct TpmRequest : BaseRequest
{
    string keyName;
    string ownerAuth;
    string srkAuth;
    string dekAuth;
    string data;
    string fileName;
    int index;
    int dataSize;       
    int type;

    TpmRequest() : index(0),dataSize(0),type(-1) {}
};

struct TpmPostRequest : BaseRequest
{
    int dataSize;
    string ownerAuth;
    string srkAuth;
    string dekAuth;
    string data;
    string objectName;
    TpmPostRequest() : dataSize(0) {}
};

struct TpmBaseReponse
{
    int status;
    string errMsg;
    TpmBaseReponse() : status(1) {}
};

struct TpmClearResponse : TpmBaseReponse
{
    vector<string> keys;
    TpmClearResponse()
    {
        keys.clear();
    }
};

struct MqttErrorReponse
{
    int status;
    Json::Value payload;
    string errMsg;

    MqttErrorReponse() : status(0) {}
    MqttErrorReponse(const int statusCode, const Json::Value &payloadStr, const string &errMsgStr)
    {
        status = statusCode;
        payload = payloadStr;
        errMsg = errMsgStr;
    }
};

struct MqttResponse
{
    int status;
    string requestId;
    std::string actionType;
    Json::Value result;
    string errMsg;

    MqttResponse(string request_id, const std::string &action_type) : status(1), requestId(request_id), actionType(action_type) {}
};

struct MqttAck
{
    int status;
    string payload;
    string errMsg;
};

#endif