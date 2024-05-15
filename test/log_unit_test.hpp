#ifndef LOG_UNIT_TEST_HPP
#define LOG_UNIT_TEST_HPP

#include <gtest/gtest.h>
#include "log_collector/LogProxy.hpp"

struct LOG_TEST : public testing::Test
{
    const string config_file_path = "/home/champ/scm/ConfigService/schedule.ConfigService";
    std::unique_ptr<LogProxy> log;
    LogEntity g_entity;
    entity_parser parser;
    ConfigService configService;
    config_table_type table;
    
    void SetUp() {  
        if ((configService.readIniConfigFile(config_file_path, table) == SCM::FAILED))
        {
            return;
        }
        log = std::make_unique<LogProxy>(table);
        g_entity = parser.getLogEntity(table, "syslog");
    }
    void TearDown() {  }
};

// TEST_F(LOG_TEST,ValidateLogEntityWithValidInput)
// {
//     LogEntity entity; 
//     entity.format = "syslog"; 
//     entity.name = "syslog"; 
//     entity.read_path = "/var/log/syslog, /var/log/dpkg.log, /var/log/auth.log";
//     entity.write_path = "/etc/scl/log";
//     entity.json_attributes; 
//     entity.time_pattern = "0 */1 * * * *"; 
//     entity.storage_type = "json"; 
//     entity.log_levels; 
//     entity.count = 0;
//     entity.delimeter = ',';
//     entity.remote = 'n'; 
//     entity.is_empty = true;
//     entity.last_read_time;
//     entity.current_read_time;
//     bool response = log->validateLogEntity(entity);
//     EXPECT_TRUE(response);
// }

TEST_F(LOG_TEST, InvalidReadPath)
{
    LogEntity entity = g_entity;
    entity.read_path = "";
    bool response = log->validateLogEntity(entity);
    EXPECT_FALSE(response);
}

TEST_F(LOG_TEST, InvalidWritePath)
{
    LogEntity entity = g_entity;
    entity.write_path = "vwrcewq";
    bool response = log->validateLogEntity(entity);
    EXPECT_FALSE(response);
}

TEST_F(LOG_TEST, InvalidTimePattern)
{
    LogEntity entity = g_entity;
    entity.time_pattern = "* * ";
    bool response = log->validateLogEntity(entity);
    EXPECT_TRUE(response);
}
TEST_F(LOG_TEST, InvalidStorageType)
{
    LogEntity entity = g_entity;
    entity.storage_type ="csv";
    bool response = log->validateLogEntity(entity);
    EXPECT_TRUE(response);
}

// TEST_F(LOG_TEST,GetPreviousLogReadTimeWithValidInput)
// {
//     LogEntity entity; 
//     entity.format = "syslog"; 
//     entity.name = "syslog"; 
//     entity.read_path = "/var/log/syslog, /var/log/dpkg.log, /var/log/auth.log";
//     entity.write_path = "/etc/scl/log";
//     entity.json_attributes; 
//     entity.time_pattern = "0 */1 * * * *"; 
//     entity.storage_type = "json"; 
//     entity.log_levels; 
//     entity.count = 0;
//     entity.delimeter = ',';
//     entity.remote = 'n'; 
//     entity.is_empty = true;
//     entity.last_read_time;
//     entity.current_read_time;
//     bool response = log->getPreviousLogReadTime(entity);
//     EXPECT_TRUE(response);
// }


#endif