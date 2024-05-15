// #include <gtest/gtest.h>
// #include "util/ConfigService.hpp"
// //#include "mock/oc_mock.hpp"
// using namespace SCM;

// class os_TEST : public testing::Test
// {
// protected:
//     //Mockos mock_os;
//     const string TEMP_WORK_DIR = "/home/champ/Desktop/sample/";
//     void SetUp()
//     {
//         auto today = std::chrono::system_clock::now();
//         auto timeInfo = std::chrono::system_clock::to_time_t(today);
//         std::tm *tm_info = std::localtime(&timeInfo);
//         int day = tm_info->tm_mday;
//         os::current_day = day; /* Current day at the application starting date. */
//         os::current_month = tm_info->tm_mon;
//         os::current_year = tm_info->tm_year + 1900;
//     }
//     void TearDown() {}
// };

// TEST_F(os_TEST, CreatesDirectoryWithValidInput)
// {
//     const string &dir_name = "/home/champ/Desktop/sample/";
//     int response = os::createDir(dir_name);
//     EXPECT_EQ(response, SUCCESS);
//     bool result = std::filesystem::exists(dir_name);
//     EXPECT_TRUE(result);
//     if (result){
//         std::filesystem::remove_all(dir_name);
//     }
// }

// TEST_F(os_TEST, FailsToCreateDirectoryWithEmptyStringInput)
// {
//     const string dir_name;
//     int response = os::createDir(dir_name);
//     EXPECT_EQ(response, FAILED);
// }

// TEST_F(os_TEST, FailsToCreateParentChildDirectories)
// {
//     const string dir_name = "/home/champ/Desktop/parent/child";
//     int response = os::createDir(dir_name);
//     EXPECT_EQ(response, FAILED);
// }

// TEST_F(os_TEST, SuccessfullyCreatesDirectoryIfNotExists)
// {
//     const string &dir_name = "/home/champ/Desktop/";
//     int response = os::createDir(dir_name);
//     EXPECT_EQ(response, SUCCESS);
// }


// TEST_F(os_TEST, DirectoryDoesNotExistWithEmptyStringInput)
// {
//     const string &dir_name = " ";
//     bool response = os::isDirExist(dir_name);
//     EXPECT_FALSE(response);
// }
// TEST_F(os_TEST, DirectoryDoesNotExistWithIncorrectDirectoryPath)
// {
//     const string &dir_name = "/home/champDesktop/";
//     bool response = os::isDirExist(dir_name);
//     EXPECT_FALSE(response);
// }
// TEST_F(os_TEST, DirectoryExistWithCorrectDirectoryPathInput)
// {
//     const string &dir_name = "/home/champ/Desktop/";
//     bool response = os::isDirExist(dir_name);
//     EXPECT_TRUE(response);
// }

// TEST_F(os_TEST, CreateFileWithValidFilePathInput)
// {
//     const string file_path = "/home/champ/Desktop/test_sample.txt";
//     int response = os::createFile(file_path);
//     EXPECT_EQ(response, SUCCESS);
//     std::filesystem::remove(file_path); //set to original state
// }
// TEST_F(os_TEST, CreateFileWithParentDirectoryInput)
// {
//     const string file_path = "/home/champ/Desktop/sample/test_sample.txt";
//     int response = os::createFile(file_path);
//     EXPECT_EQ(response, SUCCESS);
//     std::filesystem::remove_all("/home/champ/Desktop/sample/"); //set to original state
// }
// TEST_F(os_TEST, CreateFileWithEmptyStringInput)
// {
//     const string file_path = "";
//     int response = os::createFile(file_path);
//     EXPECT_EQ(response, FAILED);
// }
// TEST_F(os_TEST, CreateFileWithParentChileDirectoryInput)
// {
//     const string file_path = "/home/champ/Desktop/parent/child/test_sample.txt";
//     int response = os::createFile(file_path);
//     EXPECT_EQ(response, FAILED);
// }

// TEST_F(os_TEST, FileExistWithCorrectFilePathInput)
// {
//     const string &file = "/home/champ/Desktop/scm/test/os_test.hpp";
//     bool response = os::isFileExist(file);
//     EXPECT_TRUE(response);
// }

// TEST_F(os_TEST, FileExistWithCorrectFilePathWithTrailingAndEndingSpacesInput)
// {
//     const string &file = " /home/champ/Desktop/scm/test/os_test.hpp ";
//     bool response = os::isFileExist(file);
//     EXPECT_TRUE(response);
// }

// TEST_F(os_TEST, FileDoesNotExistWithInCorrectFilePathInput)
// {
//     const string &file = "/home/champ/Desktop/sm/test/os_test.hpp";
//     bool response = os::isFileExist(file);
//     EXPECT_FALSE(response);
// }

// TEST_F(os_TEST, FileDoesNotExistWithEmpyStringFilePathInput)
// {
//     const string &file = "";
//     bool response = os::isFileExist(file);
//     EXPECT_FALSE(response);
// }


// TEST_F(os_TEST, DeleteFileWithValidFilePathInput)
// {
//     const string file_name = "/home/champ/Desktop/scm/test/spber.grew";
//     int result = os::createFile(file_name);
//     ASSERT_EQ(result,SUCCESS);
//     int response = os::deleteFile(file_name);
//     EXPECT_EQ(response, SUCCESS);
// }
// TEST_F(os_TEST, DeleteFileWithValidFileWithTrailingAndEndingSpacesPathInput)
// {

//     const string file_name = " /home/champ/Desktop/scm/test/spber.grew";
//     int result = os::createFile(file_name);
//     ASSERT_EQ(result,SUCCESS);
//     int response = os::deleteFile(file_name);
//     EXPECT_EQ(response, SUCCESS);
// }
// TEST_F(os_TEST, DeleteFileWithInCorrectFilePathInput)
// {
//     const string file_name = "/home/champ/Desktop/scm/test/sper.grew";
//     int response = os::deleteFile(file_name);
//     EXPECT_EQ(response, FAILED);
// }
// TEST_F(os_TEST, DeleteFileWithEmptyStringFilePathInput)
// {
//     const string file_name = " ";
//     int response = os::deleteFile(file_name);
//     EXPECT_EQ(response, FAILED);
// }

// TEST_F(os_TEST, DoNotCreateLogArchiveFileWithEmptyStringInputAppName)
// {
//     string file_path;
//     const string &app_name = "";
//     int response = os::createLogArchiveFile(os::current_day, os::current_month, os::current_year, file_path, app_name);
//     EXPECT_EQ(response, FAILED);
// }

// TEST_F(os_TEST, DoNotCreateLogArchiveFileWithNonEmptySpaceStringInputAppName)
// {
//     string file_path;
//     const string &app_name = "  ";
//     int response = os::createLogArchiveFile(os::current_day, os::current_month, os::current_year, file_path, app_name);
//     EXPECT_EQ(response, FAILED);
// }

// /*
// // TEST_F(os_TEST, TestHandleLocalLogFile)
// // {
// //     // Arrange
// //     int day = 1;
// //     int month = 4;
// //     int year = 2024;
// //     std::string file_path = "log_file_path";
// //     std::string app_name = "your_app_name";

// //     // Set up expectations for compressFile mock if needed
// //     // For example:
// //     // EXPECT_CALL(mock_os, compressFile(testing::_))
// //     //     .WillOnce(testing::Return(SCM::SUCCESS)); // Assuming it returns SUCCESS for testing

// //     // Set up expectations for createLogArchiveFile mock if needed
// //     // For example:
// //     // EXPECT_CALL(mock_os, createLogArchiveFile(day, month, year, testing::_, app_name))
// //     //     .WillOnce(testing::Return(SCM::SUCCESS)); // Assuming it returns SUCCESS for testing

// //     // Act
// //     int result = os::handleLocalLogFile(day, month, year, file_path, app_name);

// //     // Assert
// //     // Write your assertions based on the expected behavior of your function
// //     // For example:
// //     // EXPECT_EQ(result, EXPECTED_RESULT);
// // }
// */

// TEST_F(os_TEST, GetFileNameOrBackUpFileNameOFLogTypeWithSysLogPath)
// {
//     string filename = "/var/log/syslog";
//     static string response = os::getPathOrBackupFilePath(filename);
//     EXPECT_STREQ(response.c_str(), "/var/log/syslog");
//     //EXPECT_EQ(response,SUCCESS);
// }

// TEST_F(os_TEST, GetFileNameOrBackUpFileNameOFLogTypeWithDpkgLogPath)
// {
//     string filename = "/var/log/dpkg.log";
//     static string response = os::getPathOrBackupFilePath(filename);
//     EXPECT_STREQ(response.c_str(), "/var/log/dpkg.log.1");
//     //EXPECT_EQ(response,SUCCESS);
// }

// TEST_F(os_TEST, GetFileNameOrBackUpFileNameOFLogTypeWithNonSyslogPath)
// {
//     string filename = "/var/log/agent.log";
//     static string response = os::getPathOrBackupFilePath(filename);
//     EXPECT_STRNE(response.c_str(), "/var/log/agent.log.1");
//     //EXPECT_EQ(response,SUCCESS);
// }


// TEST_F(os_TEST, CompressFileWithCorrectValidString)
// {
//     int result = os::createDir(TEMP_WORK_DIR);
//     ASSERT_EQ(result, SUCCESS);
    
//     const string &logFile = TEMP_WORK_DIR + "sample.txt";
//     result = os::createFile(logFile);
//     ASSERT_EQ(result, SUCCESS);
//     std::fstream file(logFile, std::ios::out);
//     file << "cjnbwuhvcwirohfwpo\n";
//     file.close();
//     int response = os::compressFile(logFile);
//     ASSERT_EQ(response,SUCCESS);
//     std::filesystem::remove_all(TEMP_WORK_DIR);
// }

// TEST_F(os_TEST, CompressFileWithInvaliFilePathStringInput)
// {
//     int result = os::createDir(TEMP_WORK_DIR);
//     ASSERT_EQ(result, SUCCESS);
//     const string &logFile = "/home/champ/Desktop/sample/smple.txt";
//     int response = os::compressFile(logFile);
//     EXPECT_EQ(response,FAILED);
// }

// TEST_F(os_TEST, CompressDirectoryWithvaliFilePathStringInput)
// {
//     const string &logFile = "/home/champ/Downloads/Pravin";
//     int response = os::compressFile(logFile);
//     EXPECT_EQ(response,FAILED);
// }

// TEST_F(os_TEST, GetRegularFilesWithCorrectDirectoryPath)
// {
//     const string &directory = "/home/champ/Desktop/scm";
//     vector<string> files;
//     int response = os::getRegularFiles(directory,files);
//     ASSERT_EQ(response,SUCCESS);
//     for (string f: files) {std::cout << f << '\n';}
// }

// TEST_F(os_TEST, GetRegularFilesWithCorrectDirectoryPathWithTrailingAndEndingWhiteSpace)
// {
//     const string &directory = "  /home/champ/Desktop/scm ";
//     vector<string> files;
//     int response = os::getRegularFiles(directory,files);
//     ASSERT_EQ(response,SUCCESS);
//     for (string f: files) {std::cout << f << '\n';}
// }

// TEST_F(os_TEST, GetRegularFilesWithEmptyStringDirectoryPath)
// {
//     const string &directory = "";
//     vector<string> files;
//     int response = os::getRegularFiles(directory,files);
//     ASSERT_EQ(response,FAILED);
//     for (string f: files) {std::cout << f << '\n';}
// }

// TEST_F(os_TEST, GetRegularFilesWithInCorrectDirectoryPath)
// {
//     const string &directory = "/home/champ/Desktop/cm";
//     vector<string> files;
//     int response = os::getRegularFiles(directory,files);
//     ASSERT_EQ(response,FAILED);
//     for (string f: files) {std::cout << f << '\n';}
// }


// TEST_F(os_TEST, GetArchiveFilePathByCurrentDayAndAppNameWithValidDay)
// {
//     const string &type = "syslog";
//     string file_path;
//     int result = os::createLogArchiveFile(os::current_day, os::current_month, os::current_year, file_path, type);
//     ASSERT_EQ(result, SUCCESS);
//     string expected_result = "/etc/scl/log/archives/2024/Apr/" + std::to_string(os::current_day) + "-" + type;
//     string response = os::getArchiveByDayAndLogName(os::current_day, type);
//     EXPECT_STREQ(response.c_str(), expected_result.c_str());
// }

// TEST_F(os_TEST, GetArchiveFilePathByCurrentDayAndAppNameWithDay0)
// {
//     const string &type = "syslog";
//     string response = os::getArchiveByDayAndLogName(0, type);
//     EXPECT_STREQ(response.c_str(), "");
// }
// TEST_F(os_TEST, GetArchiveFilePathByCurrentDayAndAppNameWithDay32)
// {
//     const string &type = "syslog";
//     string response = os::getArchiveByDayAndLogName(32, type);
//     EXPECT_STREQ(response.c_str(), "");
// }


// TEST_F(os_TEST, CreateJsonFilePathForLogTypeSyslog)
// {
//     const string &type = "syslog";
//     string expected_result = "/etc/scl/log/json/" + Common::trim(type) + "/"+os::getCurrentTimeWithNoSpace() + ".json";
//     string response = os::createJSONFile(type);
//     ASSERT_STREQ(response.c_str(), expected_result.c_str());
// }

// TEST_F(os_TEST, CreateJsonFilePathForLogTypedpkg)
// {
//     const string &type = " dpkg";table
//     string expected_result = "/etc/scl/log/json/" + Common::trim(type) + "/"+os::getCurrentTimeWithNoSpace() + ".json";
//     string response = os::createJSONFile(type);
//     ASSERT_STREQ(response.c_str(), expected_result.c_str());
// }

// TEST_F(os_TEST, CreateJsonFilePathForEmptyStringLogType)
// {
//     const string &type = "";
//     string response = os::createJSONFile(type);
//     ASSERT_STREQ(response.c_str(), "");
// }