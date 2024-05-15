#ifndef Common_UNIT_TEST_HPP
#define Common_UNIT_TEST_HPP
#pragma once 
#include "util/Common.hpp"
#include <gtest/gtest.h>

struct Common_test: public testing::Test
{
    void SetUp(){}
    void TearDown(){}
};
/*
TEST_F(Common_test, setupLogger)
{
    int result = Common::setupLogger(SCM::Config::LOG_PATH);
    ASSERT_EQ(result, SCM::SUCCESS);
    std::filesystem::remove(SCM::Config::LOG_PATH);
}

TEST_F(Common_test, TrimTrailingSpaces)
{
    string line = "  address.";
    string response = Common::trim(line);
    EXPECT_STREQ(response.c_str(), "address.");
}

TEST_F(Common_test, TrimEndingSpaces)
{
    string line = "address.  ";
    string response = Common::trim(line);
    EXPECT_STREQ(response.c_str(), "address.");
}

TEST_F(Common_test, TrimTrailingAndEndingSpaces)
{
    string line = "  address.  ";
    string response = Common::trim(line);
    EXPECT_STREQ(response.c_str(), "address.");
}

TEST_F(Common_test, TrimOnlyWhiteSpaceString)
{
    string line = "   ";
    string response = Common::trim(line);
    EXPECT_STREQ(response.c_str(), "");
}

TEST_F(Common_test, TrimEmptyString)
{
    string line = "";
    string response = Common::trim(line);
    EXPECT_STREQ(response.c_str(), "");
}


TEST_F(Common_test, backupLogFile_with_invalid_path)
{
    int result = Common::backupLogFile(SCM::Config::LOG_PATH);
    EXPECT_EQ(result, SCM::FAILED);
}

TEST_F(Common_test, CheckFileNotGreaterThanMaxSize)
{
    const string &log_file = "/var/log/dpkg.log";
    bool response = Common::isFileGreaterThanMaxSize(log_file);
    EXPECT_TRUE(response);
}

TEST_F(Common_test, CheckFileNotGreaterThanMaxSizeFails)  //Failed test
{
    const string &log_file = "";
    bool response = Common::isFileGreaterThanMaxSize(log_file);
    EXPECT_FALSE(response);
}

TEST_F(Common_test, ToLowerCaseStringWithValidInput)
{
    string str = "AGENT";
    string response = Common::toLowerCase(str);
    EXPECT_STREQ(response.c_str(), "agent");
}

TEST_F(Common_test, ToLowerCaseStringWithEmptyInput)
{
    string str = "";
    string response = Common::toLowerCase(str);
    EXPECT_STREQ(response.c_str(), "");
}

TEST_F(Common_test, CreateLogCacheFileWithSyslog)
{
    const string app_name = "syslog";
    string expected_output = "/etc/scl/tmp/" + app_name;
    string response = Common::createLogCacheFile(app_name);
    EXPECT_STREQ(response.c_str(), expected_output.c_str());
}

TEST_F(Common_test, CreateLogCacheFileWithDpkgLog)
{
    const string app_name = "dpkg";
    string expected_output = "/etc/scl/tmp/" + app_name;
    string response = Common::createLogCacheFile(app_name);
    EXPECT_STREQ(response.c_str(), expected_output.c_str());
}

TEST_F(Common_test, CreateLogCacheFileWithEmptyString)
{
    const string app_name = "";
    string response = Common::createLogCacheFile(app_name);
    EXPECT_STREQ(response.c_str(), "");
}

TEST_F(Common_test, UpdateLogWrittenTimeWithLogTypeSysLog)
{
    const string app_name = "syslog";
    const string time = "2024-03-11 16:04:38";
    int response = Common::updateLogWrittenTime(app_name, time);
    EXPECT_EQ(response, SCM::SUCCESS);
}

TEST_F(Common_test, UpdateLogWrittenTimeWithLogTypeDpkgLog)
{
    const string app_name = "dpkg";
    const string time = "2024-03-11 16:04:38";
    int response = Common::updateLogWrittenTime(app_name, time);
    EXPECT_EQ(response, SCM::SUCCESS);
}

TEST_F(Common_test, UpdateLogWrittenTimeFailsWithEmptyString)
{ 
    const string app_name = "";
    const string time; //UTC format
    int response = Common::updateLogWrittenTime(app_name, time);
    EXPECT_EQ(response, SCM::FAILED);
}
TEST_F(Common_test, UpdateLogWrittenTimeFailsWithNoTime)
{ 
    const string app_name = "syslog";
    const string time; //UTC format
    int response = Common::updateLogWrittenTime(app_name, time);
    EXPECT_EQ(response, SCM::FAILED);
}

TEST_F(Common_test, CheckValidSysLogTimeStringWithCorrectStringInput)
{
    const string sys_time = "Apr 19 11:41:58";
    bool response = Common::isValidSysLogTimeString(sys_time);
    EXPECT_TRUE(response);
}

TEST_F(Common_test, CheckValidSysLogTimeStringWithCorrectStringInput1)
{
    const string sys_time = "Apr  9 11:41:58";
    bool response = Common::isValidSysLogTimeString(sys_time);
    EXPECT_TRUE(response);
}
TEST_F(Common_test, CheckValidSysLogTimeStringWithInCorrectDateStringInput1)
{
    const string sys_time = "Apr  19 11:41:58";
    bool response = Common::isValidSysLogTimeString(sys_time);
    EXPECT_FALSE(response);
}

TEST_F(Common_test, CheckValidSysLogTimeStringWithInCorrectDateStringInput2)
{
    const string sys_time = "Apr  9 11:41:58";
    bool response = Common::isValidSysLogTimeString(sys_time);
    EXPECT_TRUE(response);
}

TEST_F(Common_test, CheckValidSysLogTimeStringWithInCorrectMonthStringInput)
{
    const string sys_time = "04 9 11:41:58";
    bool response = Common::isValidSysLogTimeString(sys_time);
    EXPECT_FALSE(response);
}

TEST_F(Common_test, CheckValidSysLogTimeStringWithInCorrectDateStringInput)
{
    const string sys_time = "Apr  Nine 11:41:58";
    bool response = Common::isValidSysLogTimeString(sys_time);
    EXPECT_FALSE(response);
}

TEST_F(Common_test, CheckValidSysLogTimeStringWithInCorrectTimeStringInput)
{
    const string sys_time = "Apr  9 11:41";
    bool response = Common::isValidSysLogTimeString(sys_time);
    EXPECT_FALSE(response);
}

TEST_F(Common_test, CheckValidSysLogTimeStringWithIncorrectStringInput)
{
    const string sys_time = "2024-03-11 16:04:38";
    bool response = Common::isValidSysLogTimeString(sys_time);
    EXPECT_FALSE(response);
}

TEST_F(Common_test, CheckValidSysLogTimeStringWithEmptyStringInput)
{
    const string sys_time = "";
    bool response = Common::isValidSysLogTimeString(sys_time);
    EXPECT_FALSE(response);
}

TEST_F(Common_test, ConvertSysLogTimeFormatToUtcTimeFormat)
{
    const std::string sys_time = "Apr  9 11:41:58"; 
    std::string utc_time;
    int response = Common::convertSysLogToUtcFormat(sys_time, utc_time);
    EXPECT_EQ(response, SCM::SUCCESS);
}

TEST_F(Common_test, ConvertSysLogTimeFormatToUtcTimeFormatWithIncorrectMonthInputString)
{
    const std::string sys_time = " 9 11:41:58"; 
    std::string utc_time;
    int response = Common::convertSysLogToUtcFormat(sys_time, utc_time);
    EXPECT_EQ(response, SCM::FAILED);
}

TEST_F(Common_test, ConvertSysLogTimeFormatToUtcTimeFormatWithIncorrectDateInputString)
{
    const std::string sys_time = "Apr  11:41:58"; 
    std::string utc_time;
    int response = Common::convertSysLogToUtcFormat(sys_time, utc_time);
    EXPECT_EQ(response, SCM::FAILED);
}

TEST_F(Common_test, ConvertSysLogTimeFormatToUtcTimeFormatWithIncorrectTimeInputString)
{
    const std::string sys_time = "Apr 9 11"; 
    std::string utc_time;
    int response = Common::convertSysLogToUtcFormat(sys_time, utc_time);
    EXPECT_EQ(response, SCM::FAILED);
}

TEST_F(Common_test, ConvertSysLogTimeFormatToUtcTimeFormatWithEmptyInputString)
{
    const std::string sys_time = ""; 
    std::string utc_time;
    int response = Common::convertSysLogToUtcFormat(sys_time, utc_time);
    EXPECT_EQ(response, SCM::FAILED);
}
*/
// TEST_F(Common_test, WriteLogWithValidInputString)
// {
//     const string log = "/var/log/syslog.log";
//     void response = Common::writeLog(log);
//     EXPECT_EQ(response, SCM::SUCCESS);
// }

// TEST_F(Common_test, WriteLogWithInValidInputString)
// {
//     const string log = "var/lg/ath.log";
//     void response = Common::writeLog(log);
//     EXPECT_EQ(response, SCM::FAILED);
// }

// TEST_F(Common_test, WriteLogWithValidEmptyString)
// {
//     const string log = "";
//     void response = Common::writeLog(log);
//     EXPECT_EQ(response, SCM::FAILED);
// }

// TEST_F(Common_test, ConvertStringToTimetWithValidInput)
// {
//     const string datetime = "2024-03-11 16:04:38";
//     long response = Common::stringToTime(datetime);
//     EXPECT_EQ(response, "2024-03-11 16:04:38");
// }

// TEST_F(Common_test, ConvertStringToTimetWithInValidInput)
// {
//     const string datetime = "2024/03/11 16-04-38";
//     long response = Common::stringToTime(datetime);
//     EXPECT_NE(response, "2024-03-11 16:04:38");
// }

// TEST_F(Common_test, GetFileSizeWithValidInput)
// {
//     const string file_name = "/home/champ/Desktop/sample.txt";
//     long response = os::getFileSize(file_name);
//     EXPECT_EQ(response,SCM::SUCCESS);
// }

// TEST_F(Common_test, GetFileSizeWithInValidInput)
// {
//     const string file_name = "/home/chamDeskop/sample.txt";
//     long response = os::getFileSize(file_name);
//     EXPECT_EQ(response,SCM::FAILED);
// }

// TEST_F(Common_test, GetFileSizeWithEmptyInput)
// {
//     const string file_name = "";
//     long response = os::getFileSize(file_name);
//     EXPECT_EQ(response,SCM::FAILED);
// }


#endif