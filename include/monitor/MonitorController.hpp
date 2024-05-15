#ifndef MONITOR_CONTROLLER_HPP
#define MONITOR_CONTROLLER_HPP
#pragma once
 
#include "monitor/MonitorProxy.hpp"
#include "util/ConfigService.hpp"
 
class MonitorController 
{
public:
	MonitorController(const string& config_file) 
		: service(std::make_unique<MonitorProxy>())
	{
		is_valid_ConfigService = (ConfigService_c.readIniConfigFile(config_file, config_table) == SCM::SUCCESS) ? true : false;
	}
 
	void start() {
		if (!is_valid_ConfigService) { return; }
		service->getAppResourceDetails(const_cast<config_table_type &>(config_table));
	}
 
private:
	ConfigService ConfigService_c;
	config_table_type config_table;
	std::unique_ptr<MonitorService> service;
	bool is_valid_ConfigService;
 
};
 
#endif // !MONITOR_CONTROLLER_HPP