#ifndef IDS_CONTROLLER_HPP
#define IDS_CONTROLLER_HPP
#pragma once
 
#include "util/config.hpp"
#include "ids/ids_service.hpp"
#include "ids/ids_proxy.hpp"
 
class ids_controller {
public:
    ids_controller(const string& config_file) : service(std::make_unique<ids_proxy>())
    {
        is_config_file_valid = (config_service.read_ini_config_file(config_file) == Audit::SUCCESS ? true : false);
    }
 
    void start() {
        if (!is_config_file_valid) { return;  }
        service->start();
    }
 
private:
    bool thread_handler;
    bool is_config_file_valid;
    std::unique_ptr<ids_service> service;
    vector<std::thread> threads;
    config_table_type config_table;
    config config_service;
};
 
#endif // !IDS_CONTROLLER_HPP