#ifndef LOG_UNIT_TEST_HPP
#define LOG_UNIT_TEST_HPP

#include <gtest/gtest.h>
#include "log_collector/log_proxy.hpp"

struct LOG_TEST : public testing::Test
{
    const string config_file_path = "/home/champ/scm/config/schedule.config";
    std::unique_ptr<log_proxy> log;
    log_entity g_entity;
    entity_parser parser;
    config config_s;
    config_table_type table;
    
    void SetUp() {  
        if ((config_s.read_ini_config_file(config_file_path, table) == Audit::FAILED))
        {
            return;
        }
        log = std::make_unique<log_proxy>(table);
        g_entity = parser.get_log_entity(table, "syslog");
    }
    void TearDown() {  }
};

// TEST_F(LOG_TEST,ValidateLogEntityWithValidInput)
// {
//     log_entity entity; 
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
//     bool response = log->validate_log_entity(entity);
//     EXPECT_TRUE(response);
// }

TEST_F(LOG_TEST, InvalidReadPath)
{
    log_entity entity = g_entity;
    entity.read_path = "";
    bool response = log->validate_log_entity(entity);
    EXPECT_FALSE(response);
}

TEST_F(LOG_TEST, InvalidWritePath)
{
    log_entity entity = g_entity;
    entity.write_path = "vwrcewq";
    bool response = log->validate_log_entity(entity);
    EXPECT_FALSE(response);
}

TEST_F(LOG_TEST, InvalidTimePattern)
{
    log_entity entity = g_entity;
    entity.time_pattern = "* * ";
    bool response = log->validate_log_entity(entity);
    EXPECT_TRUE(response);
}
TEST_F(LOG_TEST, InvalidStorageType)
{
    log_entity entity = g_entity;
    entity.storage_type ="csv";
    bool response = log->validate_log_entity(entity);
    EXPECT_TRUE(response);
}

// TEST_F(LOG_TEST,GetPreviousLogReadTimeWithValidInput)
// {
//     log_entity entity; 
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
//     bool response = log->get_previous_log_read_time(entity);
//     EXPECT_TRUE(response);
// }


#endif