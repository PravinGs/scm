#ifndef IdsService_HPP
#define IdsService_HPP
#pragma once
 
#include "util/Common.hpp"
#include "ids/IdsEntity.hpp"
class IdsService {
public:
	virtual int start(AnalysisEntity& entity) = 0;
	virtual int start(config_table_type& config_table) = 0;
	virtual int rootkitAnalysis() = 0;
	virtual ~IdsService() {
	}
};
#endif // !IdsService_HPP