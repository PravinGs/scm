#ifndef IDS_TEST_HPP
#define IDS_TEST_HPP
 
#include <gtest/gtest.h>
#include "ids/IdsServiceImpl.hpp"
 
struct IDS_TEST : public testing::Test
{
    IdsServiceImpl* ids;
    void SetUp() { ids = new IdsServiceImpl(); }
    void TearDown() { delete ids; }
};
 
// TEST_F(IDS_TEST, ReadConfigServiceFilesWithValidInput)
// {
//     const string decoder_path = "/etc/scl/decoder/decoder.xml";
//     const string rules_path = "/etc/scl/rules";
//     void response = ids->readConfigServiceFiles(decoder_path, rules_path);
//     EXPECT_EQ(response, SCM::SUCCESS);
// }
 
// TEST_F(IDS_TEST, ReadConfigServiceFilesWithInValidDecoderPath)
// {
//     const string decoder_path = "/etc/scldeder/decoder.xml";
//     const string rules_path = "/etc/scl/rules";
//     void response = ids->readConfigServiceFiles(decoder_path, rules_path);
//     EXPECT_EQ(response, SCM::FAILED);
// }
 
// TEST_F(IDS_TEST, ReadConfigServiceFilesWithInValidRulesPath)
// {
//     const string decoder_path = "/etc/scl/decor/decoder.xml";
//     const string rules_path = "/etc/sclruls";
//     void response = ids->readConfigServiceFiles(decoder_path, rules_path);
//     EXPECT_EQ(response, SCM::FAILED);
// }
 
// TEST_F(IDS_TEST, ReadConfigServiceFilesWithEmptyInput)
// {
//     const string decoder_path = "";
//     const string rules_path = "";
//     void response = ids->readConfigServiceFiles(decoder_path, rules_path);
//     EXPECT_EQ(response, SCM::FAILED);
// }
 
// TEST_F(IDS_TEST, CheckValidSyslogWithValidInput)
// {
//     size_t size = 512;
//     int response = ids->isValidSyslog(size);
//     EXPECT_EQ(response, SCM::SUCCESS);
// }
 
// TEST_F(IDS_TEST, CheckValidSyslogWithInValidInput)
// {
//     size_t size = 1028;
//     int response = ids->isValidSyslog(size);
//     EXPECT_EQ(response, SCM::FAILED);
// }


// TEST_F(IDS_TEST, StartWithValidInput)
// {
//     AnalysisEntity entity;
//     entity.log_path = "/etc/scl/log/archive/2023/Dec";
//     entity.decoder_path = "/etc/scl/decoder/decoder.xml";
//     entity.rules_path = "/etc/scl/rules";
//     entity.write_path = "/etc/scl/log/json";
//     entity.time_pattern = "0 */10 * * * *";
//     entity.storage_type = "json";
//     int response = ids->start(entity);
//     EXPECT_EQ(response, SCM::SUCCESS);
// }
 
// TEST_F(IDS_TEST, StartWithInvalidLogPath)
// {
//     AnalysisEntity entity;
//     entity.log_path = "/etc/sl/lg/archive/2023/Dec";
//     entity.decoder_path = "/etc/scl/decoder/decoder.xml";
//     entity.rules_path = "/etc/scl/rules";
//     entity.write_path = "/etc/scl/log/json";
//     entity.time_pattern = "0 */10 * * * *";
//     entity.storage_type = "json";
//     int response = ids->start(entity);
//     EXPECT_EQ(response, SCM::FAILED);
// }
 
// TEST_F(IDS_TEST, StartWithInValidDecoderPath)
// {
//     AnalysisEntity entity;
//     entity.log_path = "/etc/scl/log/archive/2023/Dec";
//     entity.decoder_path = "/etc/scldecder/decoder.xml";
//     entity.rules_path = "/etc/scl/rules";
//     entity.write_path = "/etc/scl/log/json";
//     entity.time_pattern = "0 */10 * * * *";
//     entity.storage_type = "json";
//     int response = ids->start(entity);
//     EXPECT_EQ(response, SCM::FAILED);
// }
 
// TEST_F(IDS_TEST, StartWithInvalidRulesPath)
// {
//     AnalysisEntity entity;
//     entity.log_path = "/etc/scl/log/archive/2023/Dec";
//     entity.decoder_path = "/etc/scl/decoder/decoder.xml";
//     entity.rules_path = "/etc/sl/ruls";
//     entity.write_path = "/etc/scl/log/json";
//     entity.time_pattern = "0 */10 * * * *";
//     entity.storage_type = "json";
//     int response = ids->start(entity);
//     EXPECT_EQ(response, SCM::FAILED);
// }
 
// TEST_F(IDS_TEST, StartWithInvalidWritePath)
// {
//     AnalysisEntity entity;
//     entity.log_path = "/etc/scl/log/archive/2023/Dec";
//     entity.decoder_path = "/etc/scl/deder/decoder.xml";
//     entity.rules_path = "/etc/scl/rules";
//     entity.write_path = "";
//     entity.time_pattern = "0 */10 * * * *";
//     entity.storage_type = "json";
//     int response = ids->start(entity);
//     EXPECT_EQ(response, SCM::FAILED);
// }
 
// TEST_F(IDS_TEST, StartWithInvalidTimePattern)
// {
//     AnalysisEntity entity;
//     entity.log_path = "/etc/scl/log/archive/2023/Dec";
//     entity.decoder_path = "/etc/scl/deder/decoder.xml";
//     entity.rules_path = "/etc/scl/rules";
//     entity.write_path = "/etc/scl/log/json";
//     entity.time_pattern = "0 ;*/ * * * *";
//     entity.storage_type = "json";
//     int response = ids->start(entity);
//     EXPECT_EQ(response, SCM::FAILED);
// }

/*
TEST_F(IDS_TEST, AnalyseFileWithValidInput)
{
    const string file = " ";
    int response = ids->analyseFile(file);
    EXPECT_EQ(response, SCM::SUCCESS);
}
 
TEST_F(IDS_TEST, AnalyseFileWithInValidInput)
{
    const string file = " ";
    int response = ids->analyseFile(file);
    EXPECT_EQ(response, SCM::FAILED);
}
 
TEST_F(IDS_TEST, AddDecoderCacheWithValidInput)
{
    const string decoder = "/etc/scl/decoder/decoder.xml";
    void response = ids->addDecoderCache(decoder);
    EXPECT_EQ(response, SCM::SUCCESS);
}
 
TEST_F(IDS_TEST, AddDecoderCacheWithInValidInput)
{
    const string decoder = "/etc/scldeder/decoder.xml";
    void response = ids->addDecoderCache(decoder);
    EXPECT_EQ(response, SCM::FAILED);
}
*/
 
TEST_F(IDS_TEST, DecodeLogWithValidSyslog)
{
    const string log = "Apr 16 11:27:00 linux systemd[1]: Finished Rotate log files.";
    const string format = "syslog";
    LogEvent response = ids->decodeLog(log, format);
    EXPECT_EQ(response, SCM::SUCCESS);
}
 
TEST_F(IDS_TEST, DecodeLogWithValidDpkgLog)
{
    const string log = "2024-03-11 16:04:38 startup archives unpack";
    const string format = "dpkg";
    LogEvent response = ids->decodeLog(log, format);
    EXPECT_EQ(response, SCM::SUCCESS);
}

TEST_F(IDS_TEST, DecodeLogWithInValidLog)
{
    const string log = "";
    const string format = "syslog";
    LogEvent response = ids->decodeLog(log, format);
    EXPECT_EQ(response, SCM::FAILED);
}
 
TEST_F(IDS_TEST, DecodeLogWithInValidFormat)
{
    const string log = "Apr 16 11:27:00 linux systemd[1]: Finished Rotate log files.";
    const string format = "dpkg";
    LogEvent response = ids->decodeLog(log, format);
    EXPECT_EQ(response, SCM::FAILED);
}

/*
TEST_F(IDS_TEST, FormatSyslogWithValidInput)
{
    const string log = "/etc/scl/log/archive/2023/Dec/syslog.log";     //SYSLOG_FILE = "/var/log/agent.log"
    const string format = "syslog";
    string expected_result = "/etc/scl/log/archive/2023/Dec/syslog.log" + format;      // Common::trim(format)
    string response = ids->formatSyslog(log, format);
    ASSERT_STREQ(response.c_str(), expected_result.c_str());
}
 
TEST_F(IDS_TEST, FormatSyslogWithInValidlogPath)
{
    const string log = "/etc/scl/log/archivec/syslog.log";   //SYSLOG_FILE = "/var/log/agent.log"
    const string format = "syslog";
    string expected_result = "/etc/scl/log/archive/2023/Dec/syslog.log" + format;      // Common::trim(format)
    string response = ids->formatSyslog(log, format);
    ASSERT_STRNE(response.c_str(), expected_result.c_str());
}
 
TEST_F(IDS_TEST, FormatSyslogWithInValidFormat)
{
    const string log = "/etc/scl/log/archive/2023/Dec/syslog.log";      //SYSLOG_FILE = "/var/log/agent.log"
    const string format = "lg";
    string expected_result = "/etc/scl/log/archive/2023/Dec/syslog.log" + format;      // Common::trim(format)
    string response = ids->formatSyslog(log, format);
    ASSERT_STRNE(response.c_str(), expected_result.c_str());
}
 
TEST_F(IDS_TEST, CheckRuleIdWithValidInput)
{
    const int ruleId = 2902;
    int response = ids->isRuleFound(ruleId);
    EXPECT_EQ(response, SCM::SUCCESS);
}
 
TEST_F(IDS_TEST, CheckRuleIdWithInValidInput)
{
    const int ruleId = 202;
    int response = ids->isRuleFound(ruleId);
    EXPECT_EQ(response, SCM::FAILED);
}
 
TEST_F(IDS_TEST, CheckRuleIdWithEmptyInput)
{
    const int ruleId =;
    int response = ids->isRuleFound(ruleId);
    EXPECT_EQ(response, SCM::FAILED);
}
 
TEST_F(IDS_TEST, RegexMatchWithValidInput)
{
    const string log = " ";
    const string pattern = " ";
    string match = " ";
    int response = ids->regexMatch(log, pattern, match);
    EXPECT_EQ(response, SCM::SUCCESS);
}
 
TEST_F(IDS_TEST, PcreMatchWithValidInput)
{
    const string input = " ";
    const string pattern = " ";
    string match = " ";
    size_t position;
    int response = ids->pcreMatch(input, pattern, match, position);
    EXPECT_EQ(response, SCM::SUCCESS);
}
 
TEST_F(IDS_TEST, AddMatchedRuleWithValidInput)
{
    const IdRule rule;
    rule.id = ;
    rule.group = " ";
    const string log = " ";
    void response = ids->addMatchedRule(rule, log);
    EXPECT_EQ(response, SCM::SUCCESS);
}
 
TEST_F(IDS_TEST, MatchWithValidInput)
{
    const string log = "Apr 16 11:27:00 linux systemd[1]: Finished Rotate log files.";
    const string format = "syslog";
    LogEvent response = ids->decodeLog(log, format);
    void response = ids->match(LogEvent); 
    EXPECT_EQ(response, SCM::SUCCESS);
}

TEST_F(IDS_TEST, MatchWithInValidInput)
{
    const string log = "";
    const string format = "syslog";
    LogEvent response = ids->decodeLog(log, format);
    void response = ids->match(LogEvent); 
    EXPECT_EQ(response, SCM::FAILED);
}

TEST_F(IDS_TEST, MatchWithInValidFormat)
{
    const string log = "Apr 16 11:27:00 linux systemd[1]: Finished Rotate log files.";
    const string format = "dpkg";
    LogEvent response = ids->decodeLog(log, format);
    void response = ids->match(LogEvent); 
    EXPECT_EQ(response, SCM::FAILED);
}
*/
#endif