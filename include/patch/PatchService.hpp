#ifndef PatchService_HPP
#define PatchService_HPP
 
#pragma once
#include "util/Common.hpp"
#include "patch/PatchEntity.hpp"
 
class PatchService {
public:
	virtual int start(PatchEntity& entity) = 0;
	virtual ~PatchService() {}
};
#endif //PatchService_HPP