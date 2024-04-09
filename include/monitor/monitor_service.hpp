#ifndef MONITOR_SERVICE_HPP
#define MONITOR_SERVICE_HPP
#pragma once
#include "util/common.hpp"
#include "monitor/process_entity.hpp"
class monitor_service {
public:
	virtual int get_app_resource_details(const process_entity& entity, vector<process_data>& logs) = 0;
};
#endif // !MONITOR_SERVICE_HPP