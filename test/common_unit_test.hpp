#ifndef COMMON_UNIT_TEST_HPP
#define COMMON_UNIT_TEST_HPP
#pragma once 
#include "util/common.hpp"
#include <gtest/gtest.h>

struct common_test: public testing::Test
{
    void SetUp(){}
    void TearDown(){}
};
/*
TEST_F(common_test, setup_logger)
{
    int result = common::setup_logger(Audit::Config::LOG_PATH);
    ASSERT_EQ(result, Audit::SUCCESS);
    std::filesystem::remove(Audit::Config::LOG_PATH);
}

TEST_F(common_test, TrimTrailingSpaces)
{
    string line = "  address.";
    string response = common::trim(line);
    EXPECT_STREQ(response.c_str(), "address.");
}

TEST_F(common_test, TrimEndingSpaces)
{
    string line = "address.  ";
    string response = common::trim(line);
    EXPECT_STREQ(response.c_str(), "address.");
}

TEST_F(common_test, TrimTrailingAndEndingSpaces)
{
    string line = "  address.  ";
    string response = common::trim(line);
    EXPECT_STREQ(response.c_str(), "address.");
}

TEST_F(common_test, TrimOnlyWhiteSpaceString)
{
    string line = "   ";
    string response = common::trim(line);
    EXPECT_STREQ(response.c_str(), "");
}

TEST_F(common_test, TrimEmptyString)
{
    string line = "";
    string response = common::trim(line);
    EXPECT_STREQ(response.c_str(), "");
}


TEST_F(common_test, backup_log_file_with_invalid_path)
{
    int result = common::backup_log_file(Audit::Config::LOG_PATH);
    EXPECT_EQ(result, Audit::FAILED);
}

TEST_F(common_test, CheckFileNotGreaterThanMaxSize)
{
    const string &log_file = "/var/log/dpkg.log";
    bool response = common::is_file_greater_than_max_size(log_file);
    EXPECT_TRUE(response);
}

TEST_F(common_test, CheckFileNotGreaterThanMaxSizeFails)  //Failed test
{
    const string &log_file = "";
    bool response = common::is_file_greater_than_max_size(log_file);
    EXPECT_FALSE(response);
}

TEST_F(common_test, ToLowerCaseStringWithValidInput)
{
    string str = "AGENT";
    string response = common::to_lower_case(str);
    EXPECT_STREQ(response.c_str(), "agent");
}

TEST_F(common_test, ToLowerCaseStringWithEmptyInput)
{
    string str = "";
    string response = common::to_lower_case(str);
    EXPECT_STREQ(response.c_str(), "");
}

TEST_F(common_test, CreateLogCacheFileWithSyslog)
{
    const string app_name = "syslog";
    string expected_output = "/etc/scl/tmp/" + app_name;
    string response = common::create_log_cache_file(app_name);
    EXPECT_STREQ(response.c_str(), expected_output.c_str());
}

TEST_F(common_test, CreateLogCacheFileWithDpkgLog)
{
    const string app_name = "dpkg";
    string expected_output = "/etc/scl/tmp/" + app_name;
    string response = common::create_log_cache_file(app_name);
    EXPECT_STREQ(response.c_str(), expected_output.c_str());
}

TEST_F(common_test, CreateLogCacheFileWithEmptyString)
{
    const string app_name = "";
    string response = common::create_log_cache_file(app_name);
    EXPECT_STREQ(response.c_str(), "");
}

TEST_F(common_test, UpdateLogWrittenTimeWithLogTypeSysLog)
{
    const string app_name = "syslog";
    const string time = "2024-03-11 16:04:38";
    int response = common::update_log_written_time(app_name, time);
    EXPECT_EQ(response, Audit::SUCCESS);
}

TEST_F(common_test, UpdateLogWrittenTimeWithLogTypeDpkgLog)
{
    const string app_name = "dpkg";
    const string time = "2024-03-11 16:04:38";
    int response = common::update_log_written_time(app_name, time);
    EXPECT_EQ(response, Audit::SUCCESS);
}

TEST_F(common_test, UpdateLogWrittenTimeFailsWithEmptyString)
{ 
    const string app_name = "";
    const string time; //UTC format
    int response = common::update_log_written_time(app_name, time);
    EXPECT_EQ(response, Audit::FAILED);
}
TEST_F(common_test, UpdateLogWrittenTimeFailsWithNoTime)
{ 
    const string app_name = "syslog";
    const string time; //UTC format
    int response = common::update_log_written_time(app_name, time);
    EXPECT_EQ(response, Audit::FAILED);
}

TEST_F(common_test, CheckValidSysLogTimeStringWithCorrectStringInput)
{
    const string sys_time = "Apr 19 11:41:58";
    bool response = common::is_valid_syslog_time_string(sys_time);
    EXPECT_TRUE(response);
}

TEST_F(common_test, CheckValidSysLogTimeStringWithCorrectStringInput1)
{
    const string sys_time = "Apr  9 11:41:58";
    bool response = common::is_valid_syslog_time_string(sys_time);
    EXPECT_TRUE(response);
}
TEST_F(common_test, CheckValidSysLogTimeStringWithInCorrectDateStringInput1)
{
    const string sys_time = "Apr  19 11:41:58";
    bool response = common::is_valid_syslog_time_string(sys_time);
    EXPECT_FALSE(response);
}

TEST_F(common_test, CheckValidSysLogTimeStringWithInCorrectDateStringInput2)
{
    const string sys_time = "Apr  9 11:41:58";
    bool response = common::is_valid_syslog_time_string(sys_time);
    EXPECT_TRUE(response);
}

TEST_F(common_test, CheckValidSysLogTimeStringWithInCorrectMonthStringInput)
{
    const string sys_time = "04 9 11:41:58";
    bool response = common::is_valid_syslog_time_string(sys_time);
    EXPECT_FALSE(response);
}

TEST_F(common_test, CheckValidSysLogTimeStringWithInCorrectDateStringInput)
{
    const string sys_time = "Apr  Nine 11:41:58";
    bool response = common::is_valid_syslog_time_string(sys_time);
    EXPECT_FALSE(response);
}

TEST_F(common_test, CheckValidSysLogTimeStringWithInCorrectTimeStringInput)
{
    const string sys_time = "Apr  9 11:41";
    bool response = common::is_valid_syslog_time_string(sys_time);
    EXPECT_FALSE(response);
}

TEST_F(common_test, CheckValidSysLogTimeStringWithIncorrectStringInput)
{
    const string sys_time = "2024-03-11 16:04:38";
    bool response = common::is_valid_syslog_time_string(sys_time);
    EXPECT_FALSE(response);
}

TEST_F(common_test, CheckValidSysLogTimeStringWithEmptyStringInput)
{
    const string sys_time = "";
    bool response = common::is_valid_syslog_time_string(sys_time);
    EXPECT_FALSE(response);
}

TEST_F(common_test, ConvertSysLogTimeFormatToUtcTimeFormat)
{
    const std::string sys_time = "Apr  9 11:41:58"; 
    std::string utc_time;
    int response = common::convert_syslog_to_utc_format(sys_time, utc_time);
    EXPECT_EQ(response, Audit::SUCCESS);
}

TEST_F(common_test, ConvertSysLogTimeFormatToUtcTimeFormatWithIncorrectMonthInputString)
{
    const std::string sys_time = " 9 11:41:58"; 
    std::string utc_time;
    int response = common::convert_syslog_to_utc_format(sys_time, utc_time);
    EXPECT_EQ(response, Audit::FAILED);
}

TEST_F(common_test, ConvertSysLogTimeFormatToUtcTimeFormatWithIncorrectDateInputString)
{
    const std::string sys_time = "Apr  11:41:58"; 
    std::string utc_time;
    int response = common::convert_syslog_to_utc_format(sys_time, utc_time);
    EXPECT_EQ(response, Audit::FAILED);
}

TEST_F(common_test, ConvertSysLogTimeFormatToUtcTimeFormatWithIncorrectTimeInputString)
{
    const std::string sys_time = "Apr 9 11"; 
    std::string utc_time;
    int response = common::convert_syslog_to_utc_format(sys_time, utc_time);
    EXPECT_EQ(response, Audit::FAILED);
}

TEST_F(common_test, ConvertSysLogTimeFormatToUtcTimeFormatWithEmptyInputString)
{
    const std::string sys_time = ""; 
    std::string utc_time;
    int response = common::convert_syslog_to_utc_format(sys_time, utc_time);
    EXPECT_EQ(response, Audit::FAILED);
}
*/
// TEST_F(common_test, WriteLogWithValidInputString)
// {
//     const string log = "/var/log/syslog.log";
//     void response = common::write_log(log);
//     EXPECT_EQ(response, Audit::SUCCESS);
// }

// TEST_F(common_test, WriteLogWithInValidInputString)
// {
//     const string log = "var/lg/ath.log";
//     void response = common::write_log(log);
//     EXPECT_EQ(response, Audit::FAILED);
// }

// TEST_F(common_test, WriteLogWithValidEmptyString)
// {
//     const string log = "";
//     void response = common::write_log(log);
//     EXPECT_EQ(response, Audit::FAILED);
// }

// TEST_F(common_test, ConvertStringToTimetWithValidInput)
// {
//     const string datetime = "2024-03-11 16:04:38";
//     long response = common::string_to_time_t(datetime);
//     EXPECT_EQ(response, "2024-03-11 16:04:38");
// }

// TEST_F(common_test, ConvertStringToTimetWithInValidInput)
// {
//     const string datetime = "2024/03/11 16-04-38";
//     long response = common::string_to_time_t(datetime);
//     EXPECT_NE(response, "2024-03-11 16:04:38");
// }

// TEST_F(common_test, GetFileSizeWithValidInput)
// {
//     const string file_name = "/home/champ/Desktop/sample.txt";
//     long response = os::get_file_size(file_name);
//     EXPECT_EQ(response,Audit::SUCCESS);
// }

// TEST_F(common_test, GetFileSizeWithInValidInput)
// {
//     const string file_name = "/home/chamDeskop/sample.txt";
//     long response = os::get_file_size(file_name);
//     EXPECT_EQ(response,Audit::FAILED);
// }

// TEST_F(common_test, GetFileSizeWithEmptyInput)
// {
//     const string file_name = "";
//     long response = os::get_file_size(file_name);
//     EXPECT_EQ(response,Audit::FAILED);
// }


#endif