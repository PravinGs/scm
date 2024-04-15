#ifndef MONITOR_CONTROLLER_HPP
#define MONITOR_CONTROLLER_HPP
#pragma once
 
#include "monitor/monitor_proxy.hpp"
#include "util/config.hpp"
 
class monitor_controller 
{
public:
	monitor_controller(const string& config_file) 
		: service(std::make_unique<monitor_proxy>())
	{
		is_valid_config = (config_c.read_ini_config_file(config_file, config_table) == Audit::SUCCESS) ? true : false;
	}
 
	void start() {
		if (!is_valid_config) { return; }
		service->get_app_resource_details(config_table);
	}
 
private:
	config config_c;
	config_table_type config_table;
	std::unique_ptr<monitor_service> service;
	bool is_valid_config;
 
};
 
#endif // !MONITOR_CONTROLLER_HPP