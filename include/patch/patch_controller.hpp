#ifndef PATCH_CONTROLLER_HPP
#define PATCH_CONTROLLER_HPP
 
#pragma once
#include "patch/patch_proxy.hpp"
 
class patch_controller {
public:
    patch_controller(const string& patch_controller): service(std::make_unique<patch_proxy>())
    {
        is_valid_config = (config_c.read_ini_config_file(config_file, config_table) == Audit::SUCCESS) ? true : false;
    }
 
    void start()
    {
        service->start(config_table);
    }
private:
    std::unique_ptr<patch_service> service = nullptr;
    config_table_type config_table;
    bool is_valid_config;
};
 
#endif //PATCH_CONTROLLER_HPP