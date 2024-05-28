#ifndef LOG_CONTROLLER_INTEGRATION_TEST_HPP
#define LOG_CONTROLLER_INTEGRATION_TEST_HPP

#include <gtest/gtest.h>

#include "log_collector/LogController.hpp"

struct LOG_INTEGRATION_TEST : public testing::Test
{
    ConfigService configService;
    entity_parser entityParser;
    LogServiceImpl logService;
    LogRepository logRepository;
    void SetUp() { }
    void TearDown() { }
};

// TEST_F(LOG_INTEGRATION_TEST, READ_AUTH_FILE)
// {
//     /*
//         Input
//         1. config file

//         steps
//         1. Read and build config table
//         2. Validate config table
//         3. Create Log entity
//         4. Validate Log entity
//         5. Get the last read time.
//         6. Read the file 
//         7. Update the last read time
//         8. Write and save the log data to JSON file
//         output
//         1.log file
//     */
//     const string configFilePath = "/home/champ/scm/config/schedule.config";
//     config_table_type configTable;
//     int result = SCM::SUCCESS;
//     //1.Read and build the config table
//     result = configService.readIniConfigFile(configFilePath, configTable);
//     // 2. Validate config table
//     ASSERT_EQ(result, SCM::SUCCESS); // Abbor the flow if it fails.
//     // 3. Create Log entity
//     LogEntity logEntity = entityParser.getLogEntity(configTable, "syslog");
//     logEntity.name = "auth";
//     //  4. Validate Log entity
//     LogProxy logProxy(configTable);
//     bool validationResponse = logProxy.validateLogEntity(logEntity);
//     ASSERT_TRUE(validationResponse);
//     // 5. Get the last read time.
//     validationResponse = logProxy.getPreviousLogReadTime(logEntity);
//     ASSERT_TRUE(validationResponse);
//     //6. Read the file 
//     vector<string> logs;
//     result = logService.getSysLog(logEntity, logs);
//     ASSERT_EQ(result, SCM::SUCCESS);
//     // 7. Update the last read time
//     result = Common::updateLogWrittenTime(logEntity.name, logEntity.current_read_time);
//     EXPECT_EQ(result, SCM::SUCCESS);
//     //8. Write and save the log data to JSON file
//     result = logRepository.save(logEntity, logs);
//     ASSERT_EQ(result, SCM::SUCCESS); 
//     //output
// }

// TEST_F(LOG_INTEGRATION_TEST, INVALID_CONFIG_FILE)
// {
//     const string configFilePath = "/home/champ/scm/confighedule.config";
//     config_table_type configTable;
//     int result = SCM::SUCCESS;
//     result = configService.readIniConfigFile(configFilePath, configTable);
//     ASSERT_EQ(result, SCM::SUCCESS); // Abort the flow if it fails. 
//     LogEntity logEntity = entityParser.getLogEntity(configTable, "syslog");
//     logEntity.name = "auth";
//     LogProxy logProxy(configTable);
//     bool validationResponse = logProxy.validateLogEntity(logEntity);
//     ASSERT_TRUE(validationResponse);
//     validationResponse = logProxy.getPreviousLogReadTime(logEntity);
//     ASSERT_TRUE(validationResponse);
//     vector<string> logs;
//     result = logService.getSysLog(logEntity, logs);
//     ASSERT_EQ(result, SCM::SUCCESS);
//     result = Common::updateLogWrittenTime(logEntity.name, logEntity.current_read_time);
//     EXPECT_EQ(result, SCM::SUCCESS);
//     result = logRepository.save(logEntity, logs);
//     ASSERT_EQ(result, SCM::SUCCESS); 
//     //output
// }

TEST_F(LOG_INTEGRATION_TEST, INVALID_LOG_FORMAT)
{
    const string configFilePath = "/home/champ/scm/config/schedule.config";
    config_table_type configTable;
    int result = SCM::SUCCESS;
    result = configService.readIniConfigFile(configFilePath, configTable);
    ASSERT_EQ(result, SCM::SUCCESS); 
    LogEntity logEntity = entityParser.getLogEntity(configTable, "");
    logEntity.name = "";
    LogProxy logProxy(configTable);
    bool validationResponse = logProxy.validateLogEntity(logEntity);
    ASSERT_TRUE(validationResponse);
    validationResponse = logProxy.getPreviousLogReadTime(logEntity);
    ASSERT_TRUE(validationResponse);
    vector<string> logs;
    result = logService.getSysLog(logEntity, logs);
    ASSERT_EQ(result, SCM::SUCCESS);
    result = Common::updateLogWrittenTime(logEntity.name, logEntity.current_read_time);
    EXPECT_EQ(result, SCM::SUCCESS);
    result = logRepository.save(logEntity, logs);
    ASSERT_EQ(result, SCM::SUCCESS); 
}

TEST_F(LOG_INTEGRATION_TEST, INVALID_READ_PATH)
{
    const string configFilePath = "/home/champ/scm/config/schedule.config";
    config_table_type configTable;
    int result = SCM::SUCCESS;
    result = configService.readIniConfigFile(configFilePath, configTable);
    ASSERT_EQ(result, SCM::SUCCESS); 
    LogEntity logEntity; 
    logEntity.read_path = "";
    logEntity = entityParser.getLogEntity(configTable, "syslog");
    LogProxy logProxy(configTable);
    bool validationResponse = logProxy.validateLogEntity(logEntity);
    ASSERT_TRUE(validationResponse);
    validationResponse = logProxy.getPreviousLogReadTime(logEntity);
    ASSERT_TRUE(validationResponse);
    vector<string> logs;
    result = logService.getSysLog(logEntity, logs);
    ASSERT_EQ(result, SCM::SUCCESS);
    result = Common::updateLogWrittenTime(logEntity.name, logEntity.current_read_time);
    EXPECT_EQ(result, SCM::SUCCESS);
    result = logRepository.save(logEntity, logs);
    ASSERT_EQ(result, SCM::SUCCESS); 
}

#endif