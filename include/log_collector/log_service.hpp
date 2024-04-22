#ifndef LOG_SERVICE_HPP
#define LOG_SERVICE_HPP
 
#include "util/common.hpp"
#include "log_collector/log_entity.hpp"
 
class log_service {
public:
	virtual int get_syslog() = 0;
	virtual int get_syslog(log_entity& entity, vector<string>& logs) = 0;
	virtual int get_applog() = 0;
	virtual ~log_service() {}
};
 
#pragma once
 
#endif // !LOG_SERVICE_HPP
 