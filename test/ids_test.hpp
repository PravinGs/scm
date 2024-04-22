#ifndef IDS_TEST_HPP
#define IDS_TEST_HPP
 
#include <gtest/gtest.h>
#include "ids/ids_service_impl.hpp"
 
struct IDS_TEST : public testing::Test
{
    ids_service_impl* ids;
    void SetUp() { ids = new ids_service_impl(); }
    void TearDown() { delete ids; }
};
 
// TEST_F(IDS_TEST, ReadConfigFilesWithValidInput)
// {
//     const string decoder_path = "/etc/scl/decoder/decoder.xml";
//     const string rules_path = "/etc/scl/rules";
//     void response = ids->read_config_files(decoder_path, rules_path);
//     EXPECT_EQ(response, Audit::SUCCESS);
// }
 
// TEST_F(IDS_TEST, ReadConfigFilesWithInValidDecoderPath)
// {
//     const string decoder_path = "/etc/scldeder/decoder.xml";
//     const string rules_path = "/etc/scl/rules";
//     void response = ids->read_config_files(decoder_path, rules_path);
//     EXPECT_EQ(response, Audit::FAILED);
// }
 
// TEST_F(IDS_TEST, ReadConfigFilesWithInValidRulesPath)
// {
//     const string decoder_path = "/etc/scl/decor/decoder.xml";
//     const string rules_path = "/etc/sclruls";
//     void response = ids->read_config_files(decoder_path, rules_path);
//     EXPECT_EQ(response, Audit::FAILED);
// }
 
// TEST_F(IDS_TEST, ReadConfigFilesWithEmptyInput)
// {
//     const string decoder_path = "";
//     const string rules_path = "";
//     void response = ids->read_config_files(decoder_path, rules_path);
//     EXPECT_EQ(response, Audit::FAILED);
// }
 
// TEST_F(IDS_TEST, CheckValidSyslogWithValidInput)
// {
//     size_t size = 512;
//     int response = ids->is_valid_syslog(size);
//     EXPECT_EQ(response, Audit::SUCCESS);
// }
 
// TEST_F(IDS_TEST, CheckValidSyslogWithInValidInput)
// {
//     size_t size = 1028;
//     int response = ids->is_valid_syslog(size);
//     EXPECT_EQ(response, Audit::FAILED);
// }


// TEST_F(IDS_TEST, StartWithValidInput)
// {
//     analysis_entity entity;
//     entity.log_path = "/etc/scl/log/archive/2023/Dec";
//     entity.decoder_path = "/etc/scl/decoder/decoder.xml";
//     entity.rules_path = "/etc/scl/rules";
//     entity.write_path = "/etc/scl/log/json";
//     entity.time_pattern = "0 */10 * * * *";
//     entity.storage_type = "json";
//     int response = ids->start(entity);
//     EXPECT_EQ(response, Audit::SUCCESS);
// }
 
// TEST_F(IDS_TEST, StartWithInvalidLogPath)
// {
//     analysis_entity entity;
//     entity.log_path = "/etc/sl/lg/archive/2023/Dec";
//     entity.decoder_path = "/etc/scl/decoder/decoder.xml";
//     entity.rules_path = "/etc/scl/rules";
//     entity.write_path = "/etc/scl/log/json";
//     entity.time_pattern = "0 */10 * * * *";
//     entity.storage_type = "json";
//     int response = ids->start(entity);
//     EXPECT_EQ(response, Audit::FAILED);
// }
 
// TEST_F(IDS_TEST, StartWithInValidDecoderPath)
// {
//     analysis_entity entity;
//     entity.log_path = "/etc/scl/log/archive/2023/Dec";
//     entity.decoder_path = "/etc/scldecder/decoder.xml";
//     entity.rules_path = "/etc/scl/rules";
//     entity.write_path = "/etc/scl/log/json";
//     entity.time_pattern = "0 */10 * * * *";
//     entity.storage_type = "json";
//     int response = ids->start(entity);
//     EXPECT_EQ(response, Audit::FAILED);
// }
 
// TEST_F(IDS_TEST, StartWithInvalidRulesPath)
// {
//     analysis_entity entity;
//     entity.log_path = "/etc/scl/log/archive/2023/Dec";
//     entity.decoder_path = "/etc/scl/decoder/decoder.xml";
//     entity.rules_path = "/etc/sl/ruls";
//     entity.write_path = "/etc/scl/log/json";
//     entity.time_pattern = "0 */10 * * * *";
//     entity.storage_type = "json";
//     int response = ids->start(entity);
//     EXPECT_EQ(response, Audit::FAILED);
// }
 
// TEST_F(IDS_TEST, StartWithInvalidWritePath)
// {
//     analysis_entity entity;
//     entity.log_path = "/etc/scl/log/archive/2023/Dec";
//     entity.decoder_path = "/etc/scl/deder/decoder.xml";
//     entity.rules_path = "/etc/scl/rules";
//     entity.write_path = "";
//     entity.time_pattern = "0 */10 * * * *";
//     entity.storage_type = "json";
//     int response = ids->start(entity);
//     EXPECT_EQ(response, Audit::FAILED);
// }
 
// TEST_F(IDS_TEST, StartWithInvalidTimePattern)
// {
//     analysis_entity entity;
//     entity.log_path = "/etc/scl/log/archive/2023/Dec";
//     entity.decoder_path = "/etc/scl/deder/decoder.xml";
//     entity.rules_path = "/etc/scl/rules";
//     entity.write_path = "/etc/scl/log/json";
//     entity.time_pattern = "0 ;*/ * * * *";
//     entity.storage_type = "json";
//     int response = ids->start(entity);
//     EXPECT_EQ(response, Audit::FAILED);
// }

/*
TEST_F(IDS_TEST, AnalyseFileWithValidInput)
{
    const string file = " ";
    int response = ids->analyse_file(file);
    EXPECT_EQ(response, Audit::SUCCESS);
}
 
TEST_F(IDS_TEST, AnalyseFileWithInValidInput)
{
    const string file = " ";
    int response = ids->analyse_file(file);
    EXPECT_EQ(response, Audit::FAILED);
}
 
TEST_F(IDS_TEST, AddDecoderCacheWithValidInput)
{
    const string decoder = "/etc/scl/decoder/decoder.xml";
    void response = ids->add_decoder_cache(decoder);
    EXPECT_EQ(response, Audit::SUCCESS);
}
 
TEST_F(IDS_TEST, AddDecoderCacheWithInValidInput)
{
    const string decoder = "/etc/scldeder/decoder.xml";
    void response = ids->add_decoder_cache(decoder);
    EXPECT_EQ(response, Audit::FAILED);
}
*/
 
TEST_F(IDS_TEST, DecodeLogWithValidSyslog)
{
    const string log = "Apr 16 11:27:00 linux systemd[1]: Finished Rotate log files.";
    const string format = "syslog";
    log_event response = ids->decode_log(log, format);
    EXPECT_EQ(response, Audit::SUCCESS);
}
 
TEST_F(IDS_TEST, DecodeLogWithValidDpkgLog)
{
    const string log = "2024-03-11 16:04:38 startup archives unpack";
    const string format = "dpkg";
    log_event response = ids->decode_log(log, format);
    EXPECT_EQ(response, Audit::SUCCESS);
}

TEST_F(IDS_TEST, DecodeLogWithInValidLog)
{
    const string log = "";
    const string format = "syslog";
    log_event response = ids->decode_log(log, format);
    EXPECT_EQ(response, Audit::FAILED);
}
 
TEST_F(IDS_TEST, DecodeLogWithInValidFormat)
{
    const string log = "Apr 16 11:27:00 linux systemd[1]: Finished Rotate log files.";
    const string format = "dpkg";
    log_event response = ids->decode_log(log, format);
    EXPECT_EQ(response, Audit::FAILED);
}

/*
TEST_F(IDS_TEST, FormatSyslogWithValidInput)
{
    const string log = "/etc/scl/log/archive/2023/Dec/syslog.log";     //SYSLOG_FILE = "/var/log/agent.log"
    const string format = "syslog";
    string expected_result = "/etc/scl/log/archive/2023/Dec/syslog.log" + format;      // common::trim(format)
    string response = ids->format_syslog(log, format);
    ASSERT_STREQ(response.c_str(), expected_result.c_str());
}
 
TEST_F(IDS_TEST, FormatSyslogWithInValidlogPath)
{
    const string log = "/etc/scl/log/archivec/syslog.log";   //SYSLOG_FILE = "/var/log/agent.log"
    const string format = "syslog";
    string expected_result = "/etc/scl/log/archive/2023/Dec/syslog.log" + format;      // common::trim(format)
    string response = ids->format_syslog(log, format);
    ASSERT_STRNE(response.c_str(), expected_result.c_str());
}
 
TEST_F(IDS_TEST, FormatSyslogWithInValidFormat)
{
    const string log = "/etc/scl/log/archive/2023/Dec/syslog.log";      //SYSLOG_FILE = "/var/log/agent.log"
    const string format = "lg";
    string expected_result = "/etc/scl/log/archive/2023/Dec/syslog.log" + format;      // common::trim(format)
    string response = ids->format_syslog(log, format);
    ASSERT_STRNE(response.c_str(), expected_result.c_str());
}
 
TEST_F(IDS_TEST, CheckRuleIdWithValidInput)
{
    const int ruleId = 2902;
    int response = ids->is_rule_found(ruleId);
    EXPECT_EQ(response, Audit::SUCCESS);
}
 
TEST_F(IDS_TEST, CheckRuleIdWithInValidInput)
{
    const int ruleId = 202;
    int response = ids->is_rule_found(ruleId);
    EXPECT_EQ(response, Audit::FAILED);
}
 
TEST_F(IDS_TEST, CheckRuleIdWithEmptyInput)
{
    const int ruleId =;
    int response = ids->is_rule_found(ruleId);
    EXPECT_EQ(response, Audit::FAILED);
}
 
TEST_F(IDS_TEST, DecodeGroupWithValidInput)
{
    log_event logEvent;
    logEvent.group = " ";
    string decode_group()
}
 
TEST_F(IDS_TEST, RegexMatchWithValidInput)
{
    const string log = " ";
    const string pattern = " ";
    string match = " ";
    int response = ids->regex_match(log, pattern, match);
    EXPECT_EQ(response, Audit::SUCCESS);
}
 
TEST_F(IDS_TEST, PcreMatchWithValidInput)
{
    const string input = " ";
    const string pattern = " ";
    string match = " ";
    size_t position;
    int response = ids->pcre_match(input, pattern, match, position);
    EXPECT_EQ(response, Audit::SUCCESS);
}
 
TEST_F(IDS_TEST, AddMatchedRuleWithValidInput)
{
    const id_rule rule;
    rule.id = ;
    rule.group = " ";
    const string log = " ";
    void response = ids->add_matched_rule(rule, log);
    EXPECT_EQ(response, Audit::SUCCESS);
}
 
TEST_F(IDS_TEST, MatchWithValidInput)
{
    const string log = "Apr 16 11:27:00 linux systemd[1]: Finished Rotate log files.";
    const string format = "syslog";
    log_event response = ids->decode_log(log, format);
    void response = ids->match(log_event); 
    EXPECT_EQ(response, Audit::SUCCESS);
}

TEST_F(IDS_TEST, MatchWithInValidInput)
{
    const string log = "";
    const string format = "syslog";
    log_event response = ids->decode_log(log, format);
    void response = ids->match(log_event); 
    EXPECT_EQ(response, Audit::FAILED);
}

TEST_F(IDS_TEST, MatchWithInValidFormat)
{
    const string log = "Apr 16 11:27:00 linux systemd[1]: Finished Rotate log files.";
    const string format = "dpkg";
    log_event response = ids->decode_log(log, format);
    void response = ids->match(log_event); 
    EXPECT_EQ(response, Audit::FAILED);
}
*/
#endif