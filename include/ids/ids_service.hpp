#ifndef IDS_SERVICE_HPP
#define IDS_SERVICE_HPP
#pragma once
 
#include "util/common.hpp"
#include "ids/ids_entity.hpp"
class ids_service {
public:
	virtual int start(analysis_entity& entity) = 0;
	virtual int start(config_table_type& config_table) = 0;
	virtual int rootkit_analysis() = 0;
	virtual ~ids_service() {
	}
};
#endif // !IDS_SERVICE_HPP