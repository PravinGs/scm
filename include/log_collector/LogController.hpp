#ifndef LOG_CONTROLLER_HPP
#define LOG_CONTROLLER_HPP
#pragma once
#include "util/ConfigService.hpp"
#include "log_collector/LogProxy.hpp"
 
class LogController {
public:
	LogController(const std::string& config_file): thread_handler(true)
	{
		is_config_file_valid = (configServiceservice.readIniConfigFile(config_file, config_table) == SCM::SUCCESS ? true : false);
		if (is_config_file_valid)
		{
			service = std::make_unique<LogProxy>(config_table);
		}
	}	
 
	int start() 
	{
		if (!is_config_file_valid) 
		{
			return SCM::FAILED;
		}
		return service->getSysLog();
	}
 
	void startAsync() 
	{
		if (!is_config_file_valid)
		{
			DEBUG("ConfigService file read successfully");
			return;
		}
		for (int i = 0; i < (int)PROCESSES.size(); i++)
		{
			string process_name = PROCESSES[i];
			try
			{
				threads[i] = std::thread([&, process_name]()
					{ assignTaskToThread(process_name); });
			}
			catch (const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
		}
	}
private:
	void assignTaskToThread(const string& process)
	{
		if (process == "applog")
		{
			//(void)service->getAppLog(config_table);
		}
		else if (process == "syslog")
		{
			(void)service->getSysLog();
		}
	}
private:
    bool thread_handler;
    bool is_config_file_valid;
    const vector<string> PROCESSES = { "syslog","applog" };
    std::unique_ptr<LogService> service;
    vector<std::thread> threads;
    config_table_type config_table;
	ConfigService configServiceservice;
};
 
#endif // !LogController_HPP