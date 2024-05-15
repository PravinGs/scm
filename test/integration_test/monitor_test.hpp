#ifndef MONITOR_CONTROLLER_INTEGRATION_TEST_HPP
#define MONITOR_CONTROLLER_INTEGRATION_TEST_HPP

#include <gtest/gtest.h>
#include "monitor/MonitorController.hpp"

struct MONITOR_INTEGRATION_TEST : public testing::Test
{
    ConfigService configService;
    entity_parser entityParser;
    MonitorServiceImpl monitorService;
    MonitorRepository monitorRepository;
    void SetUp() { }
    void TearDown() { }
};

TEST_F(MONITOR_INTEGRATION_TEST, READ_AUTH_FILE)
{
    /*
        Input
        1. config file

        steps
        1. Read and build config table
        2. Validate config table
        3. Create Process entity
        4. Validate Process entity
        5. Get the Resource details
        6. Write and save the process data to JSON file
        output
        1.log file
    */
    const string configFilePath = "/home/champ/scm/config/schedule.config";
    config_table_type configTable;
    int result = SCM::SUCCESS;
    //1.Read and build the config table
    result = configService.readIniConfigFile(configFilePath, configTable);
    // 2. Validate config table
    ASSERT_EQ(result, SCM::SUCCESS); // Abbor the flow if it fails.
    // 3. Create Process entity
    ProcessEntity processEntity = entityParser.getProcessEntity(configTable);
    //  4. Validate Process entity
    MonitorProxy monitorProxy(configTable);
    bool validationResponse = monitorProxy.validateProcessEntity(processEntity);
    ASSERT_TRUE(validationResponse);
    //  5. Get the Resource details
    vector<string> logs;
    result = monitorService.getAppResourceDetails(processEntity, logs);
    ASSERT_EQ(result, SCM::SUCCESS);
    //  6. Write and save the log data to JSON file
    const vector<ProcessData> &logs;
    const SystemProperties &properties; 
    const SystemProperties &availed_properties;
    result = monitorRepository.save(logEntity, logs);
    ASSERT_EQ(result, SCM::SUCCESS); 
    //output
}

#endif