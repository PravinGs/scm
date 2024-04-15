#ifndef PATCH_SERVICE_HPP
#define PATCH_SERVICE_HPP
 
#pragma once
#include "util/common.hpp"
#include "patch/patch_entity.hpp"
 
class patch_service {
public:
	virtual int start(patch_entity& entity) = 0;
	virtual ~patch_service() {}
};
 
#endif //PATCH_SERVICE_HPP