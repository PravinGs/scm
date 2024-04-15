#ifndef IDS_TEST_HPP
#define IDS_TEST_HPP

#include <gtest/gtest.h>
#include "ids/ids_service_impl.hpp"
 
struct IDS_TEST : public testing::Test
{
    ids_service *ids;
    void SetUp() { ids = new ids_service(); }
    void TearDown() { delete ids; }
};
 
TEST_F(IDS_TEST, ReadConfigFilesWithValidInput)
{
    const string decoder_path = "/etc/scl/decoder/decoder.xml";
    const string rules_path = "/etc/scl/rules";
    void response = ids.read_config_files(decoder_path, rules_path);
    EXPECT_EQ(response, SUCCESS);
}

TEST_F(IDS_TEST, ReadConfigFilesWithInValidDecoderPath)
{
    const string decoder_path = "/etc/scldeder/decoder.xml";
    const string rules_path = "/etc/scl/rules";
    void response = ids.read_config_files(decoder_path, rules_path);
    EXPECT_EQ(response, FAILED);
}
 
TEST_F(IDS_TEST, ReadConfigFilesWithInValidRulesPath)
{
    const string decoder_path = "/etc/scl/decor/decoder.xml";
    const string rules_path = "/etc/sclruls";
    void response = ids.read_config_files(decoder_path, rules_path);
    EXPECT_EQ(response, FAILED);
}

TEST_F(IDS_TEST, ReadConfigFilesWithEmptyInput)
{
    const string decoder_path = "";
    const string rules_path = "";
    void response = ids.read_config_files(decoder_path, rules_path);
    EXPECT_EQ(response, FAILED);
}

TEST_F(IDS_TEST, CheckValidSyslogWithValidInput)
{
    size_t size = 512;
    int response = ids.is_valid_syslog(size);
    EXPECT_EQ(response, Audit::SUCCESS);
}
 
TEST_F(IDS_TEST, CheckValidSyslogWithInValidInput)
{
    size_t size = 1028;
    int response = ids.is_valid_syslog(size);
    EXPECT_EQ(response, Audit::FAILED);
}
 
TEST_F(IDS_TEST, CheckValidSyslogWithEmptyInput)
{
    size_t size = ;
    int response = ids.is_valid_syslog(size);
    EXPECT_EQ(response, Audit::FAILED);
}

TEST_F(IDS_TEST, StartWithValidInput)
{
    analysis_entity entity;
    entity.log_path = "/etc/scl/log/archive/2023/Dec";
    entity.decoder_path = "/etc/scl/decoder/decoder.xml";
    entity.rules_path = "/etc/scl/rules";
    entity.write_path = "/etc/scl/log/json";
    entity.time_pattern = "0 */10 * * * *";
    entity.storage_type = "json";
    int response = ids.start(entity);
    EXPECT_EQ(response, Audit::SUCCESS);
}

TEST_F(IDS_TEST, StartWithInvalidLogPath)
{
    analysis_entity entity;
    entity.log_path = "/etc/sl/lg/archive/2023/Dec";
    entity.decoder_path = "/etc/scl/decoder/decoder.xml";
    entity.rules_path = "/etc/scl/rules";
    entity.write_path = "/etc/scl/log/json";
    entity.time_pattern = "0 */10 * * * *";
    entity.storage_type = "json";
    int response = ids.start(entity);
    EXPECT_EQ(response, Audit::FAILED);
}
 
TEST_F(IDS_TEST, StartWithInValidDecoderPath)
{
    analysis_entity entity;
    entity.log_path = "/etc/scl/log/archive/2023/Dec";
    entity.decoder_path = "/etc/scldecder/decoder.xml";
    entity.rules_path = "/etc/scl/rules";
    entity.write_path = "/etc/scl/log/json";
    entity.time_pattern = "0 */10 * * * *";
    entity.storage_type = "json";
    int response = ids.start(entity);
    EXPECT_EQ(response, Audit::FAILED);
}

TEST_F(IDS_TEST, StartWithInvalidRulesPath)
{
    analysis_entity entity;
    entity.log_path = "/etc/scl/log/archive/2023/Dec";
    entity.decoder_path = "/etc/scl/decoder/decoder.xml";
    entity.rules_path = "/etc/sl/ruls";
    entity.write_path = "/etc/scl/log/json";
    entity.time_pattern = "0 */10 * * * *";
    entity.storage_type = "json";
    int response = ids.start(entity);
    EXPECT_EQ(response, Audit::FAILED);
}
 
TEST_F(IDS_TEST, StartWithInvalidWritePath)
{
    analysis_entity entity;
    entity.log_path = "/etc/scl/log/archive/2023/Dec";
    entity.decoder_path = "/etc/scl/deder/decoder.xml";
    entity.rules_path = "/etc/scl/rules";
    entity.write_path = "";
    entity.time_pattern = "0 */10 * * * *";
    entity.storage_type = "json";
    int response = ids.start(entity);
    EXPECT_EQ(response, Audit::FAILED);
}
 
TEST_F(IDS_TEST, StartWithInvalidTimePattern)
{
    analysis_entity entity;
    entity.log_path = "/etc/scl/log/archive/2023/Dec";
    entity.decoder_path = "/etc/scl/deder/decoder.xml";
    entity.rules_path = "/etc/scl/rules";
    entity.write_path = "/etc/scl/log/json";
    entity.time_pattern = "0 ;*/ * * * *";
    entity.storage_type = "json";
    int response = ids.start(entity);
    EXPECT_EQ(response, Audit::FAILED);
}

TEST_F(IDS_TEST, AnalyseFileWithValidInput)
{
    const string &file = " ";
    int response = ids.analyse_file(file);
    EXPECT_EQ(response, Audit::SUCCESS);
}
 
TEST_F(IDS_TEST, AnalyseFileWithInValidInput)
{
    const string &file = " ";
    int response = ids.analyse_file(file);
    EXPECT_EQ(response, Audit::FAILED);
}

TEST_F(IDS_TEST, AddDecoderCacheWithValidInput)
{
    const string &decoder = "/etc/scl/decoder/decoder.xml";
    void response = ids.add_decoder_cache(decoder);
    EXPECT_EQ(response, SUCCESS);
}
 
TEST_F(IDS_TEST, AddDecoderCacheWithInValidInput)
{
    const string &decoder = "/etc/scldeder/decoder.xml";
    void response = ids.add_decoder_cache(decoder);
    EXPECT_EQ(response, SUCCESS);
}

TEST_F(IDS_TEST, DecodeLogWithValidInput)
{
    const string log = ""; 
    const string format = "";
    log_event response = ids.decode_log(log, format);
    EXPECT_EQ(response, )
}
 
 
 
 
 
 
 
 
 
 