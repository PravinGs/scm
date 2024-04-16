#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#pragma once

#include <string>
#include <vector>
using std::string;
using std::vector;
struct conn_entity
{
	int port;
	string conn_string;
	string ca_pem;
	string cert_pem;
	string key_pem;

	conn_entity() : port(-1) {}
};

namespace Audit
{
	static bool is_syslog_enabled = false;

	namespace Config
	{
		const string LOG_PATH = "/etc/scl/log/agent.log";
		// const string LOG_PATH = "C:/Users/pravinkumar.m/source/repos/Auditing/src/log/log.txt";
		const string BASE_LOG_DIR = "/etc/scl/log/";
		const string BASE_LOG_ARCHIVE = "archives";
		const string BASE_CONFIG_TMP = "tmp/";
		const string BASE_CONFIG_DIR = "/etc/scl/config/";
		const string AGENT_CONFIG_DIR = "/etc/scl/config/agent/agent.config";
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

	namespace Rest {
		const long POST_SUCCESS = 200L;
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

	/*void print_config_table(const config_table_type& config_table) {
		for (const auto& section : config_table) {
			std::cout << "[" << section.first << "]\n";
			for (const auto& line : section.second) {
				std::cout << line.first << " = " << line.second << '\n';
			}
			std::cout << '\n';
		}
	}*/
}

#endif // CONSTANTS_HPP