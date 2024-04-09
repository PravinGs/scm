#ifndef COMMON_HPP
#define COMMON_HPP
 
#pragma once

#include "libs.hpp"
#include "logger.hpp"
#include "os.hpp"
#include "croncpp.hpp"
 
class common {
public:
	static fstream logfp;
	static bool debug;

	
 
	static int setup_logger(const string& log_file); // default Audit::Config::
	
	static string trim(string line);
 
	static int backup_log_file(const string& file_path);
    
	static bool is_file_greater_than_max_size(const string& log_file);
 
	static string to_lower_case(string& str);
 
	static bool is_valid_syslog_time_string(const string& sys_time);
 
	static string create_log_cache_file(const string& app_name);
 
	static int update_log_written_time(const string& app_name, const string& time);
 
	static string get_hostname();
 
	static int convert_syslog_to_utc_format(const std::string& sys_time, std::string& utc_time);
 
	static void write_log(const string& log);
 
	static void write_log(const string& log, int log_level);
 
	static std::time_t string_to_time_t(const string& datetime);
 
	static void print_next_execution_time(std::tm next_time_info);
 
	static void print_duration(const std::chrono::duration<double>& duration);
 
	static std::uintmax_t get_file_size(const string& file_path);
 
	static void pause_till_next_execution(const cron::cronexpr& cron);
 
	~common()
	{
		if (logfp.is_open())
			logfp.close();
	}
};
#endif // !COMMON_HPP
//common.hpp