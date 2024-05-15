// #include <gtest/gtest.h>
// #include "util/ConfigService.hpp"
// #include <cstdlib>

// using namespace SCM;

// struct ConfigService_TEST : public testing::Test
// {
//     ConfigService cs;
//     void SetUp() { }
//     void TearDown() { }
// };

// TEST_F(ConfigService_TEST, ConvertToIntWithValidString)
// {
//     string number = "12";
//     int response = cs.toInt(number);
//     EXPECT_EQ(response, 12);
// }
// TEST_F(ConfigService_TEST, ConvertToIntWithValidStringWithTrailingAndEndingWhiteSpaces)
// {
//     string number = " 12 ";
//     int response = cs.toInt(number);
//     EXPECT_EQ(response, 12);
// }

// TEST_F(ConfigService_TEST, ConvertToIntWithEmptyString)
// {
//     string number = "";
//     int response = cs.toInt(number);
//     EXPECT_EQ(response, -1);
// }

// TEST_F(ConfigService_TEST, ConvertToIntWithWhitespaceString)
// {
//     string number = " ";
//     int response = cs.toInt(number);
//     EXPECT_EQ(response, -1);
// }
// TEST_F(ConfigService_TEST, ConvertToIntWithInvalidStringWithCharacters)
// {
//     string number = "ew";
//     int response = cs.toInt(number);
//     EXPECT_EQ(response, -1);
// }
// TEST_F(ConfigService_TEST, ConvertToIntWithInvalidStringWithNumberAndCharacters)
// {
//     string number = "2ew";
//     int response = cs.toInt(number);
//     EXPECT_EQ(response, 2);
// }
// TEST_F(ConfigService_TEST, ConvertToIntWithInvalidStringWithCharactersAndNumbers)
// {
//     string number = "ew2";
//     int response = cs.toInt(number);
//     EXPECT_EQ(response, -1);
// }

// TEST_F(ConfigService_TEST, ValidateIniFormatLineWithKeyValueString)
// {
//     string line = "client_id = paho_c++_client";
//     bool response = cs.validateIniFormat(line);
//     ASSERT_TRUE(response);
//     EXPECT_STREQ(line.c_str(), "client_id = paho_c++_client");
// }

// TEST_F(ConfigService_TEST, ValidateIniFormatLineWithInvalidCommentLine)
// {
//     string line = "client_id = ;paho_c++_client;";
//     bool response = cs.validateIniFormat(line);
//     ASSERT_FALSE(response);
// }


// TEST_F(ConfigService_TEST, ConvertCommaSeperatedValuesToVector)
// {
//     const string& columns = "TimeGenerated,UserLoginId,LogLevel,Priority,Message";
//     const char sep = ',';
//     vector<string> response = cs.toVector(columns, sep);
//     for (string s: response){
//         std::cout << s << '\n';
//     }
//     EXPECT_GT(response.size(), 0);
// }

// TEST_F(ConfigService_TEST, ConvertInvalidCommaSeperatedValuesToVector)
// {
//     const string& columns = "TimeGenerated,UserLoginId,LogLevel,Priority,Message";
//     const char sep = '.';
//     vector<string> response = cs.toVector(columns, sep);
//     EXPECT_EQ(response.size(), 0);
// }


// TEST_F(ConfigService_TEST, ReadValidInIConfigServiceFile)
// {
//     const string path = "/home/champ/Desktop/scm/ConfigService/schedule.ConfigService"; 
//     map<string, map<string, string>> table;
//     int response = cs.readIniConfigFile(path, table);
//     EXPECT_EQ(response, SUCCESS);
// }

// TEST_F(ConfigService_TEST, ReadInValidInIConfigServiceFilePath)
// {
//     const string path = "/home/champ/Desktop/scm/ConfigService/scheule.ConfigService"; 
//     map<string, map<string, string>> table;
//     int response = cs.readIniConfigFile(path, table);
//     EXPECT_EQ(response, FAILED);
// }

// TEST_F(ConfigService_TEST, ReadInValidInIConfigServiceFileWithInvalidSection)
// {
//     const string path = "/home/champ/Desktop/scm/ConfigService/scheule.ConfigService"; 
//     map<string, map<string, string>> table;
//     int response = cs.readIniConfigFile(path, table);
//     EXPECT_EQ(response, FAILED);
// }

// TEST_F(ConfigService_TEST, ReadInValidInIConfigServiceFile)
// {
//     const string path = "/home/champ/Desktop/scm/ConfigService/test_ini_ConfigService"; 
//     map<string, map<string, string>> table;
//     int response = cs.readIniConfigFile(path, table);
//     EXPECT_EQ(response, FAILED);
// }


// TEST_F(ConfigService_TEST, ReadValidDecoderFile)
// {
//     const string & file_name = "/home/champ/Desktop/scm/ConfigService/decoder.xml";
//     std::unordered_map<string, decoder> table;
//     int response = cs.readDecoder(file_name, table);
//     EXPECT_EQ(response, SUCCESS);
// }
// TEST_F(ConfigService_TEST, ReadInValidDecoderFile)
// {
//     const string & file_name = "/home/chmp/Desktop/scm/ConfigService/decoder.xml";
//     std::unordered_map<string, decoder> table;
//     int response = cs.readDecoder(file_name, table);
//     EXPECT_EQ(response, FAILED);
// }
// TEST_F(ConfigService_TEST, ReadInValidXMLFormatDecoderFile)
// {
//     const string & file_name = "/home/champ/Desktop/scm/ConfigService/test_decoder_ConfigService";
//     std::unordered_map<string, decoder> table;
//     int response = cs.readDecoder(file_name, table);
//     EXPECT_EQ(response, FAILED);
// }


// TEST_F(ConfigService_TEST, ReadAConfigServiceRulesWithValidXMLFormatFile)
// {
//     string file_name = "/etc/scl/rules/syslog_rules.xml";
//     std::unordered_map<string, std::unordered_map<int, AConfigService>> table;
//     int response = cs.readAConfig(file_name, table);
//     EXPECT_EQ(response, SUCCESS);
// }

// TEST_F(ConfigService_TEST, ReadAConfigServiceRulesWithInValidRulesFile)
// {
//     string file_name = "/etc/scl/rules/sys_ruls.xml";
//     std::unordered_map<string, std::unordered_map<int, AConfigService>> table;
//     int response = cs.readAConfig(file_name, table);
//     EXPECT_EQ(response, FAILED);
// }

// TEST_F(ConfigService_TEST, ReadDecoderConfigServiceFileWithValidInput)
// {
//     const string& path = "/etc/scl/decoder/decoder.xml";
//     std::unordered_map<string, decoder> table;
//     int response = cs.readDecoderConfigFile(path, table);
//     EXPECT_EQ(response, SUCCESS);
// }
// TEST_F(ConfigService_TEST, ReadDecoderConfigServiceFileWithInValidInput)
// {
//     const string& path = "/etc/scl/decoder";
//     std::unordered_map<string, decoder> table;
//     int response = cs.readDecoderConfigFile(path, table);
//     EXPECT_EQ(response, FAILED);
// }

// TEST_F(ConfigService_TEST, ReadXMLRulesConfigServiceFileWithValidInput)
// {
//     const string& path = "/etc/scl/rules";
//     std::unordered_map<string, std::unordered_map<int, AConfigService>> table;
//     int response = cs.readXmlRuleConfigFile(path, table);
//     EXPECT_EQ(response, SUCCESS);
// }
// TEST_F(ConfigService_TEST, ReadXMLRulesConfigServiceFileWithInValidInput)
// {
//     const string& path = "/etc/scl/ruleser";
//     std::unordered_map<string, std::unordered_map<int, AConfigService>> table;
//     int response = cs.readXmlRuleConfigFile(path, table);
//     EXPECT_EQ(response, FAILED);
// }