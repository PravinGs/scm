#ifndef MqttEntity_HPP
#define MqttEntity_HPP

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

enum class ActionType
{
    LogRequest = 0,
    PatchRequest = 1,
    ProcessRequest = 2,
    TpmConfigServiceuration = 3
};
 
enum class ResponseType
{
    MqttResponse = 0,
    RestApiResponse = 1
};
 
enum class LogPriority
{
    None = 0,
    Trace = 1,
    InteractionLevel = 2,
    StandardLevel = 3,
    Alarm = 4,
};
 
enum class LogLevel
{
    None = 0,
    Trace = 1,
    Debug = 2,
    Warning = 3,
    Error = 4,
    Critical = 5,
};
 
enum class LogEnumCategory
{
    Application = 0,
    SCM = 1,
    System = 2,
    Firewall = 3,
    Network = 4,
    Other = 5
};
 
enum class TpmCommand
{
    TpmClear = 0,
    SealKey = 1,
    UnsealKey = 2,
    NvStore = 3,
    NvRead = 4
};
 
struct BaseResponse
{
    std::string guid;
    bool is_deleted;
    std::string created_on;
    std::string modified_on;
    std::string created_by;
    std::string modified_by;
    std::string row_version;
    BaseResponse() : is_deleted(true) {}
};
 
struct LogResponse : BaseResponse
{
    std::string time_generated;
    std::string time_wriiten;   
    std::string org_id;         
    std::string app_name;
    std::string source;
    std::string message;
    LogEnumCategory category;
    std::string user_login_id;
    LogPriority priority;
    LogLevel level;
};
 
struct MqttLogResponse
{
    std::vector<LogResponse> logs;
};
 
struct BaseRequest
{
    std::string source_id;
    int ActionType;
    int is_ack_required;
    int ResponseType;
 
    BaseRequest() : ActionType(0), is_ack_required(0), ResponseType(-1) {}
    BaseRequest(const std::string &source_id, int ActionType, int is_ack_required, int ResponseType)
        : source_id(source_id), ActionType(ActionType), is_ack_required(is_ack_required), ResponseType(ResponseType) {}
};
 
struct PatchRequest : BaseRequest
{
    int distribution_type;
    std::string url;
    std::string patch_version;
    std::string username;
    std::string password;
    PatchRequest() : distribution_type(0) {}

};
 
struct LogRequest : BaseRequest
{
    std::string log_type;
    int log_level;
    std::string start_time;
    std::string end_time;
 
    LogRequest() : log_level(0)
    {
    }
};
 
struct ProcessRequest : public BaseRequest
{
    int process_id;
    std::string process_name;
    int is_SystemProperties_required;
    double ram_memory;
    double disk_memory;
    double cpu_time;
 
    ProcessRequest() : process_id(0), is_SystemProperties_required(0), ram_memory(0.0f), disk_memory(0.0f), cpu_time(0.0f)
    {
    }
};
 
struct TpmRequest : BaseRequest
{
    int command;
    TpmRequest(){}
    TpmRequest(const std::string &source_id, int ActionType, int is_ack_required, int ResponseType, int command)
        : BaseRequest{source_id, ActionType, is_ack_required, ResponseType}, command(command) {}
};
 
struct TpmConfig : TpmRequest
{
    string lockout_auth;
    TpmConfig(){}
    TpmConfig(const std::string &source_id, int ActionType, int is_ack_required, int ResponseType, int command, const std::string &lockout_auth)
        : TpmRequest{source_id, ActionType, is_ack_required, ResponseType, command}, lockout_auth(lockout_auth) {}
};
 
struct SealConfig : TpmRequest
{
    string ownerAuth;
    string srkAuth;
    string dekAuth;
    string fileName;
    string objectName;
    SealConfig() {}
    SealConfig(const std::string &source_id, int ActionType, int is_ack_required, int ResponseType, int command, const string &ownerAuth, const string &srkAuth, const string &dekAuth, const string &fileName, const string& objectName)
        : TpmRequest{source_id, ActionType, is_ack_required, ResponseType, command}, ownerAuth(ownerAuth), srkAuth(srkAuth), dekAuth(dekAuth), fileName(fileName), objectName(objectName) {}
};
 
struct PersistConfig : TpmRequest
{
    string indexName;
    string ownerPassword;
    string fileName;
    int indexValue; 
    string indexPassword; 
    int dataSize; 
    PersistConfig() {}
    PersistConfig(const std::string &source_id, int ActionType, int is_ack_required, int ResponseType, int command, const string &indexName, const string &ownerPassword, const string &fileName, const int indexValue, const string &indexPassword, const int dataSize)
        : TpmRequest{source_id, ActionType, is_ack_required, ResponseType, command}, indexName(indexName),ownerPassword(ownerPassword), fileName(fileName), indexValue(indexValue), indexPassword(indexPassword), dataSize(dataSize) {}
};
 
struct TpmDataResponse
{
};

#endif