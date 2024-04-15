#ifndef MQTT_ENTITY_HPP
#define MQTT_ENTITY_HPP

struct mqtt_entity
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
    mqtt_entity() : port(8000), qos(1), is_secure(true) {}
};

enum class action_type
{
    LogRequest = 0,
    PatchRequest = 1,
    ProcessRequest = 2,
    TpmConfiguration = 3
};
 
enum class response_type
{
    MqttResponse = 0,
    RestApiResponse = 1
};
 
enum class log_priority
{
    None = 0,
    Trace = 1,
    InteractionLevel = 2,
    StandardLevel = 3,
    Alarm = 4,
};
 
enum class log_level
{
    None = 0,
    Trace = 1,
    Debug = 2,
    Warning = 3,
    Error = 4,
    Critical = 5,
};
 
enum class log_category
{
    Application = 0,
    Audit = 1,
    System = 2,
    Firewall = 3,
    Network = 4,
    Other = 5
};
 
enum class tpm_command
{
    TpmClear = 0,
    SealKey = 1,
    UnsealKey = 2,
    NvStore = 3,
    NvRead = 4
};
 
struct base_reponse
{
    std::string guid;
    bool is_deleted;
    std::string created_on;
    std::string modified_on;
    std::string created_by;
    std::string modified_by;
    std::string row_version;
    base_reponse() : is_deleted(true) {}
};
 
struct log_response : base_reponse
{
    std::string time_generated; // generate current time in UTC format
    std::string time_wriiten;   // TimeGenerated
    std::string org_id;         //
    std::string app_name;
    std::string source;
    std::string message;
    log_category category;
    std::string user_login_id;
    log_priority priority;
    log_level level;
};
 
struct mqtt_log_response
{
    std::vector<log_response> logs;
};
 
struct base_request
{
    std::string source_id;
    int action_type;
    int is_ack_required;
    int response_type;
 
    // Constructor to initialize all fields
    base_request() : action_type(0), is_ack_required(0), response_type(-1) {}
    base_request(const std::string &source_id, int action_type, int is_ack_required, int response_type)
        : source_id(source_id), action_type(action_type), is_ack_required(is_ack_required), response_type(response_type) {}
};
 
struct patch_request : base_request
{
    int distribution_type;
    std::string url;
    std::string patch_version;
    std::string username;
    std::string password;
    patch_request() : distribution_type(0) {}
 
    // patch_request(const std::string &source_id, int action_type, int is_ack_required, int response_type, int distribution_type,
    //               const std::string& url, const std::string& patch_version, const std::string& username, const std::string& password)
    //     : base_request(source_id, action_type, is_ack_required, response_type),
    //       distribution_type(distribution_type), url(url), patch_version(patch_version), username(username), password(password) {}
 
};
 
struct log_request : base_request
{
    std::string log_type;
    int log_level;
    std::string start_time;
    std::string end_time;
 
    log_request() : log_level(0)
    {
    }
};
 
struct process_request : public base_request
{
    int process_id;
    std::string process_name;
    int is_sys_properties_required;
    double ram_memory;
    double disk_memory;
    double cpu_time;
 
    process_request() : process_id(0), is_sys_properties_required(0), ram_memory(0.0f), disk_memory(0.0f), cpu_time(0.0f)
    {
    }
};
 
struct tpm_request : base_request
{
    int command;
    tpm_request(){}
    // Constructor to initialize all fields
    tpm_request(const std::string &source_id, int action_type, int is_ack_required, int response_type, int command)
        : base_request{source_id, action_type, is_ack_required, response_type}, command(command) {}
};
 
struct tpm_config : tpm_request
{
    string lockout_auth;
    tpm_config(){}
    tpm_config(const std::string &source_id, int action_type, int is_ack_required, int response_type, int command, const std::string &lockout_auth)
        : tpm_request{source_id, action_type, is_ack_required, response_type, command}, lockout_auth(lockout_auth) {}
};
 
struct seal_config : tpm_request
{
    string ownerAuth;
    string srkAuth;
    string dekAuth;
    string fileName;
    string objectName;
    seal_config() {}
    seal_config(const std::string &source_id, int action_type, int is_ack_required, int response_type, int command, const string &ownerAuth, const string &srkAuth, const string &dekAuth, const string &fileName, const string& objectName)
        : tpm_request{source_id, action_type, is_ack_required, response_type, command}, ownerAuth(ownerAuth), srkAuth(srkAuth), dekAuth(dekAuth), fileName(fileName), objectName(objectName) {}
};
 
struct persist_config : tpm_request
{
    string indexName;
    string ownerPassword;
    string fileName;
    int indexValue; 
    string indexPassword; 
    int dataSize; 
    persist_config() {}
    persist_config(const std::string &source_id, int action_type, int is_ack_required, int response_type, int command, const string &indexName, const string &ownerPassword, const string &fileName, const int indexValue, const string &indexPassword, const int dataSize)
        : tpm_request{source_id, action_type, is_ack_required, response_type, command}, indexName(indexName),ownerPassword(ownerPassword), fileName(fileName), indexValue(indexValue), indexPassword(indexPassword), dataSize(dataSize) {}
};
 
struct tpm_data_response
{
};

#endif