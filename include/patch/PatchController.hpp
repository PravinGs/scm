#ifndef PatchController_HPP
#define PatchController_HPP
 
#pragma once
#include "patch/PatchProxy.hpp"
 
class PatchController {
public:
    PatchController(const string& PatchController): service(std::make_unique<PatchProxy>())
    {
        is_valid_ConfigService = (ConfigService_c.readIniConfigFile(config_file, config_table) == SCM::SUCCESS) ? true : false;
    }
 
    void start()
    {
        service->start(config_table);
    }
private:
    std::unique_ptr<PatchService> service = nullptr;
    config_table_type config_table;
    bool is_valid_ConfigService;
};
 
#endif //PatchController_HPP