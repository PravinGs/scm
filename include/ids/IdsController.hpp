#ifndef IdsController_HPP
#define IdsController_HPP
#pragma once

#include "util/ConfigService.hpp"
#include "ids/IdsService.hpp"
#include "ids/IdsProxy.hpp"

class IdsController
{
public:
    IdsController(const string &config_file) : service(std::make_unique<IdsProxy>())
    {
        is_config_file_valid = (config_service.readIniConfigFile(config_file, config_table) == SCM::SUCCESS ? true : false);
    }

    void start()
    {
        if (!is_config_file_valid)
        {
            return;
        }
        service->start(const_cast<config_table_type &>(config_table));
    }

    void startRootAnalysis()
    {
        if (!is_config_file_valid)
        {
            return;
        }
        service->rootkitAnalysis();
    }

private:
    bool thread_handler;
    bool is_config_file_valid;
    std::unique_ptr<IdsService> service;
    vector<std::thread> threads;
    config_table_type config_table;
    ConfigService config_service;
};

#endif // !IdsController_HPP