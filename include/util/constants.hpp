#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#pragma once

#include <string>
#include <vector>
typedef short unsigned int tpm_string_size;

using std::string;
using std::vector;

typedef

	struct conn_entity
{
	int port;
	string conn_string;
	string ca_pem;
	string cert_pem;
	string key_pem;

	conn_entity() : port(-1) {}
};

namespace SCM
{
	static bool is_syslog_enabled = false;

	const int MQTT_REQUEST_EMPTY_OR_NULL_ACTION_TYPE = 100;
	const int MQTT_REQUEST_INVALID_RESPONSE_TYPE = 101;
	const int MQTT_REQUEST_EMPTY_OR_NULL_TOPIC_ID = 102;
	const int MQTT_REQUEST_EMPTY_OR_NULL_TARGET_ID = 103;
	const int MQTT_REQUEST_EMPTY_OR_NULL_REQUEST_ID = 104;
	const int MQTT_REQUEST_INVALID_LOG_TYPE = 105;
	const int MQTT_INVALID_TPM_COMMAND = 106;

	namespace StatusCode
	{
		const int MQTT_JSON_REQUEST_PARSER_ERROR = -1;
	}

	namespace Default
	{
		const string SYSLOG_READ_PATH = "/var/log/syslog";
		const string AUTHLOG_READ_PATH = "/var/log/auth.log";
		const string DPKGLOG_READ_PATH = "/var/log/dpkg.log";
	}

	namespace Config
	{
		const string LOG_PATH = "/etc/scl/log/agent.log";
		const string BASE_LOG_DIR = "/etc/scl/log/";
		const string BASE_LOG_ARCHIVE = "archives";
		const string BASE_CONFIG_TMP = "tmp/";
		const string BASE_CONFIG_DIR = "/etc/scl/config/";
		const string AGENT_CONFIG_DIR = "/etc/scl/Config/agent/agent.config";
		const string AGENT_TEMP_DIR = "/etc/scl/tmp/";
		const string SYSLOG_FILE = "/var/log/agent.log";
		const string SEP = "/";
	}

	namespace Monitor
	{
		const string PROC = "/proc/";
		const string CPUDATA = "/stat";
		const string MEMORYDATA = "/statm";
		const string BOOTTIME = "uptime";
		const string COMM = "/comm";
		const string IO = "/io";
		const int MAX_NICE_VALUE = 20;
		const int CLK_TCK = 100;
		const int UITME = 13;
		const int STIME = 14;
		const int CUTIME = 15;
		const int CSTIME = 16;
		const int NICETIME = 18;
		const int START_TIME = 21;
	}

	namespace IDS
	{
		const string IDS_DEFAULT_XML_RULES_PATH = "/etc/scl/rules";
		const string IDS_DEFAULT_DECODER_RULES_PATH = "/etc/scl/decoder/decoder.xml";
		const string TROJAN_SOURCE_FILE = "/etc/scl/ids/rootkit_trojans.txt";
		const string SYS_SOURCE_FILE = "/etc/scl/ids/rootkit_files.txt";
	}

	namespace Patch
	{
		const string DEFAULT_DOWNLOAD_LOCATION = "/etc/scl/tmp/";
		const int SERVER_ERROR = 10;
		const int DOWNLOAD_WAITING_TIME = 5;
		const int PATCH_DEFAULT_RETRY_COUNT = 5;
		const int PATCH_DEFAULT_RETRY_TIMEOUT = 10;
		const int PATCH_DEFAULT_MAX_DOWNLOAD_SPEED = 10;
		const int PATCH_DEFAULT_MIN_DOWNLOAD_SPEED = 1;
	}

	namespace Tpm
	{
		const int TPM2_SEAL_BUFFER_SIZE = 328;
		const int TPM2_TEST_SUCCESS = 200;
		const int TPM2_ABRMD_DEVICE_ACTIVE = 201;
		const int TPM2_ABRMD_SIMULATOR_ACTIVE = 202;
		const int TPM2_ABRMD_NOT_ACTIVE = 203;
		const int TSS2_LIBRARY_NOT_EXIST = 204;
		const int TPM2_DEVICE_NOT_EXIST = 205;
		const int TPM2_SUCCESS = 0;
		const int TPM2_INVALID_FILE = 10;
		const int TPM2_INVALID_SEAL_CONTEXT = 11;
		const int TPM2_INVALID_USER_CONTEXT = 12;
		const int TPM2_INVALID_PERSISTED_CONTEXT = 13;
		const int TPM2_INVALID_HIERARCHY_AUTH = 2466;
		const int TPM2_INVALID_ESYS_CONTEXT = 458757;
		const int TPM2_INVALID_OBJECT_AUTHORIZATION = 2446;
		const int TPM2_EXCEEDED_MAX_NV_BUFFER_SIZE = 469;
		const int TPM2_EXCEEDED_MAX_SEAL_BUFFER_SIZE = 589840;
		const int TPM2_EMPTY_KEY_FILE = 706;
		const int TPM2_INVALID_OBJECT = 1;
		const int TPM2_INVALID_OBJECT_CREDENTIAL = 15;
		const int TPM2_DUPLICATE_VALUE = 102;
		const int TPM2_DATABASE_TRANSACTION_FAILED = 101;
		const int TPM2_INVALID_DB_QUERY = 105;
		const int TPM2_GENERIC_MESSAGE = 2;
		const int TPM2_INVALID_DEVICE = 3;
		const int TPM2_INVALID_DATABASE = 4;
		const int TPM2_PERSISTENT_KEY_LIMIT = 7;
		const int TPM2_DATABASE_CONSTARINT_VIOLATION = 19;
		const int TPM2_EMPTY_STRING = 20;
		const int MAX_SEAL_BUFFER_SIZE = 256;
		const int TPM2_INVALID_CONTEXT = 44;

		const string SIMULATOR_ADDRESS = "libtss2-tcti-mssim.so.0:host=127.0.0.1,port=2321";

		const string DEFAULT_DB_PATH = "/home/champ/scm/include/db/secrets";
		const string TPM_AUTHORIZATION_STORAGE = "/etc/scl/tpm/";
		const string SEAL_PREFIX = "seal_";
		const string NV_PREFIX = "nv_";
	}

	namespace Rest
	{
		const long POST_SUCCESS = 200L;
		const int ORG_ID = 101;
		const string BEARER_TOKEN = "eyJhbGciOiJIUzI1NiIsInR5cCI6ImF0K2p3dCJ9.eyJzY29wZSI6WyJlbWFpbCIsIm9wZW5pZCIsInByb2ZpbGUiLCJzZi5maXJtd2FyZS5tYW5hZ2UiXSwicm9sZSI6WyJTRkFkbWluIiwiU0ZVc2VyIl0sInBlcm1pc3Npb24iOlsic2YuYXBpUmVzb3VyY2UubWFuYWdlIiwic2YuaWRlbnRpdHlyZXNvdXJjZS5tYW5hZ2UiLCJzZi5jbGllbnQubWFuYWdlIiwic2YuYXVkaXR0cmFpbC5tYW5hZ2UiLCJzZi5yb2xlLm1hbmFnZSIsInNmLmFkbWludXNlci5tYW5hZ2UiLCJzZi51c2VyLm1hbmFnZSIsInNmLnNlY3VyaXR5dG9rZW4ubWFuYWdlIiwic2YubG9ncy5tYW5hZ2UiLCJzZi5maXJtd2FyZS5tYW5hZ2UiXSwiaXNzIjoic2VjdXJpdHkuSGNsLmNvbSIsImlhdCI6MTcxMzkzOTY2NSwic3ViIjoiZGFiYmZjZjYtZjAzYi00MWQxLWJhODItYjllMWVjMGYzYTkxIiwiZXhwIjoxNzEzOTQzMjY1LCJqdGkiOiJpUXhTTStPQnZIRCs2cTRDOTl6Wk5rNEk0SGpieW90ZFJpeWc4alJYWDlrPSIsImNsaWVudF9pZCI6InZuSVJZV0x0ZUwxRWxIT3BMZjZDRGtOMkE4VW0wQzFxL1RPMUtSU1NCd0E9IiwiaWRwIjoibG9jYWwiLCJuYmYiOjE3MTM5Mzk2NjUsImF1ZCI6InNmLmZpcm13YXJlIn0.SFZuUeeSAR9xCfYiMyxWzmLs9R7SzKuEvh9Rv0faRBY";
	}

	const int SUCCESS = 1;
	const int FAILED = -1;
	const int DEBUG = 0;
	const int INFO = 2;
	const int WARNING = 3;
	const int ERROR = 4;
	const int CRITICAL = 5;
	const int FATAL = 6;
	const int ALARM = 7;
	const int SCHEDULAR_WAIT = 11;

	const int MAX_CACHE_SIZE = 5;

	const int BUFFER_SIZE = 1024;
	const int MAX_RK_SYS = 512;
	const int OS_SIZE_1024 = 1024;
	const int PROC_ = 0;
	const int PID = 1;
	const int TASK = 2;
	const int MAX_PID = 32768;

	const vector<string> MONTHS = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	const string FILE_ERROR = "file not found or permission denied: ";
	const string CLEAN_FILE = "file truncated: ";
	const string INVALID_FILE = "invalid file: ";
	const string FCREATION_FAILED = "file creation failed: unable to create file ";
	const string FREAD_FAILED = "failed to read from file: unable to read data from ";
	const string FWRITE_FAILED = "failed to write to file: unable to write data to ";
	const string FWRITE_SUCCESS = "successfully wrote to file: wrote data to ";
	const string FDELETE_FAILED = "failed to delete a file: unable to delete  ";
	const string FDELETE_SUCCESS = "successfully deleted file: deleted ";
	const string INVALID_PATH = "path not found: ";
	const string PROMISCOUS = "ifConfig %s | grep PROMISC > /dev/null 2>&1";

	/*void print_Config_table(const Config_table_type& Config_table) {
		for (const auto& section : Config_table) {
			std::cout << "[" << section.first << "]\n";
			for (const auto& line : section.second) {
				std::cout << line.first << " = " << line.second << '\n';
			}
			std::cout << '\n';
		}
	}*/
}

#endif // CONSTANTS_HPP