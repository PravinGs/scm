#ifndef COMMON_HPP
#define COMMON_HPP

#pragma once

#include "libs.hpp"
#include "logger.hpp"
#include "os.hpp"
#include "croncpp.hpp"

typedef struct Timer Timer;

using namespace SCM;

struct Timer
{
    std::chrono::time_point<std::chrono::steady_clock> start, end;
    std::chrono::duration<float> duration;
    Timer()
    {
        start = std::chrono::steady_clock::now();
    }
    ~Timer()
    {
        end = std::chrono::steady_clock::now();
        duration = end - start;
        float ms = duration.count() * 1000.0f;
        std::cout << "Timer took " << ms << "ms\n";
    }
};

class Common
{
public:
	static fstream logfp;
	static bool debug;
	static string accessToken;
	static string dbPath;

	static int setupLogger(const string &log_file); // default SCM::Config

	static string trim(string line);

	static int backupLogFile(const string &file_path);

	static bool isFileGreaterThanMaxSize(const string &log_file);

	static string toLowerCase(string &str);

	static bool isValidSysLogTimeString(const string &sys_time);

	static string createLogCacheFile(const string &app_name);

	static int updateLogWrittenTime(const string &app_name, const string &time);

	static string getHostName();

	static int convertSysLogToUtcFormat(const std::string &sys_time, std::string &utc_time);

	static std::string convertDpkgTimeToUtcFormat(const std::string &local_time_str);

	static void writeLog(const string &log);

	static void writeLog(const string &log, int log_level);

	static std::time_t stringToTime(const string &datetime);

	static std::time_t utcStringToTime(const string& timeString);

	static void printNextExecutionTime(std::tm next_time_info);

	static void printDuration(const std::chrono::duration<double> &duration);

	static void pauseTillNextExecution(const cron::cronexpr &cron);

	static void parseArguments(int argc, char *argv[]);

	static int convertToUtc(const string &sys_time, string &utc_time);

	static vector<string> toVector(const string& line, const char sep);

	static string getErrorString(const int errorCode);

	static string generateKeyName(int type, int index);
	~Common()
	{
		if (logfp.is_open())
			logfp.close();
	}
};
#endif // COMMON_HPP
	   