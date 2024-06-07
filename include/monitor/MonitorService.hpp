#ifndef MonitorService_HPP
#define MonitorService_HPP
#pragma once
#include "util/Common.hpp"
#include "monitor/MonitorEntity.hpp"
class MonitorService
{
public:
	virtual int getAppResourceDetails(const ProcessEntity &entity, vector<ProcessData> &logs) = 0;
	virtual int getAppResourceDetails(ProcessEntity &entity, vector<ProcessData> &logs, vector<string> &processNames) = 0;
	virtual int getAppResourceDetails(config_table_type &config_table) = 0;
	virtual SystemProperties getSystemProperties() = 0;
	virtual SystemProperties getAvailedSystemProperties() = 0;
};
#endif // !MonitorService_HPP