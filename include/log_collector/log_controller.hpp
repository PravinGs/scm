#ifndef LOG_CONTROLLER_HPP
#define LOG_CONTROLLER_HPP
#pragma once
#include "util/config.hpp"
#include "log_collector/log_proxy.hpp"
 
class log_controller {
public:
	log_controller(const std::string& config_file): thread_handler(true),service(std::make_unique<log_proxy>())
	{
		is_config_file_valid = (config_service.read_ini_config_file(config_file, config_table) == Audit::SUCCESS ? true : false);
	}
 
	int start() 
	{
		if (!is_config_file_valid) 
		{
			return Audit::FAILED;
		}
		return service->get_syslog(const_cast<config_table_type &>(config_table));
	}
 
	void start_async() 
	{
		if (!is_config_file_valid)
		{
			DEBUG("Config file read successfully");
			return;
		}
		for (int i = 0; i < (int)PROCESSES.size(); i++)
		{
			string process_name = PROCESSES[i];
			try
			{
				threads[i] = std::thread([&, process_name]()
					{ assign_task_to_thread(process_name); });
			}
			catch (const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
		}
	}
private:
	void assign_task_to_thread(const string& process)
	{
		if (process == "applog")
		{
			//(void)service->get_applog(config_table);
		}
		else if (process == "syslog")
		{
			(void)service->get_syslog(config_table);
		}
	}
private:
    bool thread_handler;
    bool is_config_file_valid;
    const vector<string> PROCESSES = { "syslog","applog" };
    std::unique_ptr<log_service> service;
    vector<std::thread> threads;
    config_table_type config_table;
	config config_service;


};
 
#endif // !LOG_CONTROLLER_HPP