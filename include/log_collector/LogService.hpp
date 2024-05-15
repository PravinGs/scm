#ifndef LOG_SERVICE_HPP
#define LOG_SERVICE_HPP
 
#include "util/Common.hpp"
#include "log_collector/LogEntity.hpp"
 
class LogService {
public:
	virtual int getSysLog() = 0;
	virtual int getSysLog(LogEntity& entity, vector<string>& logs) = 0;
	virtual int getAppLog() = 0;
	virtual ~LogService() {}
};
 
#pragma once
 
#endif // !LOG_SERVICE_HPP
 