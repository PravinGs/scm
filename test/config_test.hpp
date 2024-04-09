#include <gtest/gtest.h>
#include "util/config.hpp"
#include <cstdlib>

using namespace Audit;

struct CONFIG_TEST : public testing::Test
{
    config cs;
    void SetUp() { }
    void TearDown() { }
};

TEST_F(CONFIG_TEST, ConvertToIntWithValidString)
{
    string number = "12";
    int response = cs.to_int(number);
    EXPECT_EQ(response, 12);
}
TEST_F(CONFIG_TEST, ConvertToIntWithValidStringWithTrailingAndEndingWhiteSpaces)
{
    string number = " 12 ";
    int response = cs.to_int(number);
    EXPECT_EQ(response, 12);
}

TEST_F(CONFIG_TEST, ConvertToIntWithEmptyString)
{
    string number = "";
    int response = cs.to_int(number);
    EXPECT_EQ(response, -1);
}

TEST_F(CONFIG_TEST, ConvertToIntWithWhitespaceString)
{
    string number = " ";
    int response = cs.to_int(number);
    EXPECT_EQ(response, -1);
}
TEST_F(CONFIG_TEST, ConvertToIntWithInvalidStringWithCharacters)
{
    string number = "ew";
    int response = cs.to_int(number);
    EXPECT_EQ(response, -1);
}
TEST_F(CONFIG_TEST, ConvertToIntWithInvalidStringWithNumberAndCharacters)
{
    string number = "2ew";
    int response = cs.to_int(number);
    EXPECT_EQ(response, 2);
}
TEST_F(CONFIG_TEST, ConvertToIntWithInvalidStringWithCharactersAndNumbers)
{
    string number = "ew2";
    int response = cs.to_int(number);
    EXPECT_EQ(response, -1);
}

TEST_F(CONFIG_TEST, ValidateIniFormatLineWithKeyValueString)
{
    string line = "client_id = paho_c++_client";
    bool response = cs.validate_ini_format(line);
    ASSERT_TRUE(response);
    EXPECT_STREQ(line.c_str(), "client_id = paho_c++_client");
}

TEST_F(CONFIG_TEST, ValidateIniFormatLineWithInvalidCommentLine)
{
    string line = "client_id = ;paho_c++_client;";
    bool response = cs.validate_ini_format(line);
    ASSERT_FALSE(response);
}


TEST_F(CONFIG_TEST, ConvertCommaSeperatedValuesToVector)
{
    const string& columns = "TimeGenerated,UserLoginId,LogLevel,Priority,Message";
    const char sep = ',';
    vector<string> response = cs.to_vector(columns, sep);
    for (string s: response){
        std::cout << s << '\n';
    }
    EXPECT_GT(response.size(), 0);
}

TEST_F(CONFIG_TEST, ConvertInvalidCommaSeperatedValuesToVector)
{
    const string& columns = "TimeGenerated,UserLoginId,LogLevel,Priority,Message";
    const char sep = '.';
    vector<string> response = cs.to_vector(columns, sep);
    EXPECT_EQ(response.size(), 0);
}


TEST_F(CONFIG_TEST, ReadValidInIConfigFile)
{
    const string path = "/home/champ/Desktop/scm/config/schedule.config"; 
    map<string, map<string, string>> table;
    int response = cs.read_ini_config_file(path, table);
    EXPECT_EQ(response, SUCCESS);
}

TEST_F(CONFIG_TEST, ReadInValidInIConfigFilePath)
{
    const string path = "/home/champ/Desktop/scm/config/scheule.config"; 
    map<string, map<string, string>> table;
    int response = cs.read_ini_config_file(path, table);
    EXPECT_EQ(response, FAILED);
}

TEST_F(CONFIG_TEST, ReadInValidInIConfigFileWithInvalidSection)
{
    const string path = "/home/champ/Desktop/scm/config/scheule.config"; 
    map<string, map<string, string>> table;
    int response = cs.read_ini_config_file(path, table);
    EXPECT_EQ(response, FAILED);
}

TEST_F(CONFIG_TEST, ReadInValidInIConfigFile)
{
    const string path = "/home/champ/Desktop/scm/config/test_ini_config"; 
    map<string, map<string, string>> table;
    int response = cs.read_ini_config_file(path, table);
    EXPECT_EQ(response, FAILED);
}


TEST_F(CONFIG_TEST, ReadValidDecoderFile)
{
    const string & file_name = "/home/champ/Desktop/scm/config/decoder.xml";
    std::unordered_map<string, decoder> table;
    int response = cs.read_decoder(file_name, table);
    EXPECT_EQ(response, SUCCESS);
}
TEST_F(CONFIG_TEST, ReadInValidDecoderFile)
{
    const string & file_name = "/home/chmp/Desktop/scm/config/decoder.xml";
    std::unordered_map<string, decoder> table;
    int response = cs.read_decoder(file_name, table);
    EXPECT_EQ(response, FAILED);
}
TEST_F(CONFIG_TEST, ReadInValidXMLFormatDecoderFile)
{
    const string & file_name = "/home/champ/Desktop/scm/config/test_decoder_config";
    std::unordered_map<string, decoder> table;
    int response = cs.read_decoder(file_name, table);
    EXPECT_EQ(response, FAILED);
}


TEST_F(CONFIG_TEST, ReadAConfigRulesWithValidXMLFormatFile)
{
    string file_name = "/etc/scl/rules/syslog_rules.xml";
    std::unordered_map<string, std::unordered_map<int, aconfig>> table;
    int response = cs.read_aconfig(file_name, table);
    EXPECT_EQ(response, SUCCESS);
}

TEST_F(CONFIG_TEST, ReadAConfigRulesWithInValidRulesFile)
{
    string file_name = "/etc/scl/rules/sys_ruls.xml";
    std::unordered_map<string, std::unordered_map<int, aconfig>> table;
    int response = cs.read_aconfig(file_name, table);
    EXPECT_EQ(response, FAILED);
}

TEST_F(CONFIG_TEST, ReadDecoderConfigFileWithValidInput)
{
    const string& path = "/etc/scl/decoder/decoder.xml";
    std::unordered_map<string, decoder> table;
    int response = cs.read_decoder_config(path, table);
    EXPECT_EQ(response, SUCCESS);
}
TEST_F(CONFIG_TEST, ReadDecoderConfigFileWithInValidInput)
{
    const string& path = "/etc/scl/decoder";
    std::unordered_map<string, decoder> table;
    int response = cs.read_decoder_config(path, table);
    EXPECT_EQ(response, FAILED);
}

TEST_F(CONFIG_TEST, ReadXMLRulesConfigFileWithValidInput)
{
    const string& path = "/etc/scl/rules";
    std::unordered_map<string, std::unordered_map<int, aconfig>> table;
    int response = cs.read_xml_rule_config(path, table);
    EXPECT_EQ(response, SUCCESS);
}
TEST_F(CONFIG_TEST, ReadXMLRulesConfigFileWithInValidInput)
{
    const string& path = "/etc/scl/ruleser";
    std::unordered_map<string, std::unordered_map<int, aconfig>> table;
    int response = cs.read_xml_rule_config(path, table);
    EXPECT_EQ(response, FAILED);
}