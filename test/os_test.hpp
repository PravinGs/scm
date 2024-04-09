#include <gtest/gtest.h>
#include "util/config.hpp"
//#include "mock/oc_mock.hpp"
using namespace Audit;

class OS_TEST : public testing::Test
{
protected:
    //MockOs mock_os;
    const string TEMP_WORK_DIR = "/home/champ/Desktop/sample/";
    void SetUp()
    {
        auto today = std::chrono::system_clock::now();
        auto timeInfo = std::chrono::system_clock::to_time_t(today);
        std::tm *tm_info = std::localtime(&timeInfo);
        int day = tm_info->tm_mday;
        os::current_day = day; /* Current day at the application starting date. */
        os::current_month = tm_info->tm_mon;
        os::current_year = tm_info->tm_year + 1900;
    }
    void TearDown() {}
};

TEST_F(OS_TEST, CreatesDirectoryWithValidInput)
{
    const string &dir_name = "/home/champ/Desktop/sample/";
    int response = os::create_dir(dir_name);
    EXPECT_EQ(response, SUCCESS);
    bool result = std::filesystem::exists(dir_name);
    EXPECT_TRUE(result);
    if (result){
        std::filesystem::remove_all(dir_name);
    }
}

TEST_F(OS_TEST, FailsToCreateDirectoryWithEmptyStringInput)
{
    const string dir_name = "";
    int response = os::create_dir(dir_name);
    EXPECT_EQ(response, FAILED);
}

TEST_F(OS_TEST, FailsToCreateParentChildDirectories)
{
    const string dir_name = "/home/champ/Desktop/parent/child";
    int response = os::create_dir(dir_name);
    EXPECT_EQ(response, FAILED);
}

TEST_F(OS_TEST, SuccessfullyCreatesDirectoryIfNotExists)
{
    const string &dir_name = "/home/champ/Desktop/";
    int response = os::create_dir(dir_name);
    EXPECT_EQ(response, SUCCESS);
}


TEST_F(OS_TEST, DirectoryDoesNotExistWithEmptyStringInput)
{
    const string &dir_name = " ";
    bool response = os::is_dir_exist(dir_name);
    EXPECT_FALSE(response);
}
TEST_F(OS_TEST, DirectoryDoesNotExistWithIncorrectDirectoryPath)
{
    const string &dir_name = "/home/champDesktop/";
    bool response = os::is_dir_exist(dir_name);
    EXPECT_FALSE(response);
}
TEST_F(OS_TEST, DirectoryExistWithCorrectDirectoryPathInput)
{
    const string &dir_name = "/home/champ/Desktop/";
    bool response = os::is_dir_exist(dir_name);
    EXPECT_TRUE(response);
}

TEST_F(OS_TEST, CreateFileWithValidFilePathInput)
{
    const string file_path = "/home/champ/Desktop/test_sample.txt";
    int response = os::create_file(file_path);
    EXPECT_EQ(response, SUCCESS);
    std::filesystem::remove(file_path); //set to original state
}
TEST_F(OS_TEST, CreateFileWithParentDirectoryInput)
{
    const string file_path = "/home/champ/Desktop/sample/test_sample.txt";
    int response = os::create_file(file_path);
    EXPECT_EQ(response, SUCCESS);
    std::filesystem::remove_all("/home/champ/Desktop/sample/"); //set to original state
}
TEST_F(OS_TEST, CreateFileWithEmptyStringInput)
{
    const string file_path = "";
    int response = os::create_file(file_path);
    EXPECT_EQ(response, FAILED);
}
TEST_F(OS_TEST, CreateFileWithParentChileDirectoryInput)
{
    const string file_path = "/home/champ/Desktop/parent/child/test_sample.txt";
    int response = os::create_file(file_path);
    EXPECT_EQ(response, FAILED);
}

TEST_F(OS_TEST, FileExistWithCorrectFilePathInput)
{
    const string &file = "/home/champ/Desktop/scm/test/os_test.hpp";
    bool response = os::is_file_exist(file);
    EXPECT_TRUE(response);
}

TEST_F(OS_TEST, FileExistWithCorrectFilePathWithTrailingAndEndingSpacesInput)
{
    const string &file = " /home/champ/Desktop/scm/test/os_test.hpp ";
    bool response = os::is_file_exist(file);
    EXPECT_TRUE(response);
}

TEST_F(OS_TEST, FileDoesNotExistWithInCorrectFilePathInput)
{
    const string &file = "/home/champ/Desktop/sm/test/os_test.hpp";
    bool response = os::is_file_exist(file);
    EXPECT_FALSE(response);
}

TEST_F(OS_TEST, FileDoesNotExistWithEmpyStringFilePathInput)
{
    const string &file = "";
    bool response = os::is_file_exist(file);
    EXPECT_FALSE(response);
}


TEST_F(OS_TEST, DeleteFileWithValidFilePathInput)
{
    const string file_name = "/home/champ/Desktop/scm/test/spber.grew";
    int result = os::create_file(file_name);
    ASSERT_EQ(result,SUCCESS);
    int response = os::delete_file(file_name);
    EXPECT_EQ(response, SUCCESS);
}
TEST_F(OS_TEST, DeleteFileWithValidFileWithTrailingAndEndingSpacesPathInput)
{

    const string file_name = " /home/champ/Desktop/scm/test/spber.grew";
    int result = os::create_file(file_name);
    ASSERT_EQ(result,SUCCESS);
    int response = os::delete_file(file_name);
    EXPECT_EQ(response, SUCCESS);
}
TEST_F(OS_TEST, DeleteFileWithInCorrectFilePathInput)
{
    const string file_name = "/home/champ/Desktop/scm/test/sper.grew";
    int response = os::delete_file(file_name);
    EXPECT_EQ(response, FAILED);
}
TEST_F(OS_TEST, DeleteFileWithEmptyStringFilePathInput)
{
    const string file_name = " ";
    int response = os::delete_file(file_name);
    EXPECT_EQ(response, FAILED);
}

TEST_F(OS_TEST, CreateLogArchiveFileForValidInputAppName)
{
    string file_path;
    const string &app_name = "syslog";
    int response = os::create_log_archive_file(os::current_day, os::current_month, os::current_year, file_path, app_name);
    EXPECT_EQ(response, SUCCESS);
}

TEST_F(OS_TEST, DoNotCreateLogArchiveFileWithEmptyStringInputAppName)
{
    string file_path;
    const string &app_name = "";
    int response = os::create_log_archive_file(os::current_day, os::current_month, os::current_year, file_path, app_name);
    EXPECT_EQ(response, FAILED);
}

TEST_F(OS_TEST, DoNotCreateLogArchiveFileWithNonEmptySpaceStringInputAppName)
{
    string file_path;
    const string &app_name = "  ";
    int response = os::create_log_archive_file(os::current_day, os::current_month, os::current_year, file_path, app_name);
    EXPECT_EQ(response, FAILED);
}

// TEST_F(OS_TEST, TestHandleLocalLogFile)
// {
//     // Arrange
//     int day = 1;
//     int month = 4;
//     int year = 2024;
//     std::string file_path = "log_file_path";
//     std::string app_name = "your_app_name";

//     // Set up expectations for compress_file mock if needed
//     // For example:
//     // EXPECT_CALL(mock_os, compress_file(testing::_))
//     //     .WillOnce(testing::Return(Audit::SUCCESS)); // Assuming it returns SUCCESS for testing

//     // Set up expectations for create_log_archive_file mock if needed
//     // For example:
//     // EXPECT_CALL(mock_os, create_log_archive_file(day, month, year, testing::_, app_name))
//     //     .WillOnce(testing::Return(Audit::SUCCESS)); // Assuming it returns SUCCESS for testing

//     // Act
//     int result = os::handle_local_log_file(day, month, year, file_path, app_name);

//     // Assert
//     // Write your assertions based on the expected behavior of your function
//     // For example:
//     // EXPECT_EQ(result, EXPECTED_RESULT);
// }


TEST_F(OS_TEST, GetFileNameOrBackUpFileNameOFLogTypeWithSysLogPath)
{
    string filename = "/var/log/syslog";
    static string response = os::get_path_or_backup_file_path(filename);
    EXPECT_STREQ(response.c_str(), "/var/log/syslog");
    //EXPECT_EQ(response,SUCCESS);
}

TEST_F(OS_TEST, GetFileNameOrBackUpFileNameOFLogTypeWithDpkgLogPath)
{
    string filename = "/var/log/dpkg.log";
    static string response = os::get_path_or_backup_file_path(filename);
    EXPECT_STREQ(response.c_str(), "/var/log/dpkg.log.1");
    //EXPECT_EQ(response,SUCCESS);
}

TEST_F(OS_TEST, GetFileNameOrBackUpFileNameOFLogTypeWithNonSyslogPath)
{
    string filename = "/var/log/agent.log";
    static string response = os::get_path_or_backup_file_path(filename);
    EXPECT_STRNE(response.c_str(), "/var/log/agent.log.1");
    //EXPECT_EQ(response,SUCCESS);
}


TEST_F(OS_TEST, CompressFileWithCorrectValidString)
{
    int result = os::create_dir(TEMP_WORK_DIR);
    ASSERT_EQ(result, SUCCESS);
    
    const string &logFile = TEMP_WORK_DIR + "sample.txt";
    result = os::create_file(logFile);
    ASSERT_EQ(result, SUCCESS);
    std::fstream file(logFile, std::ios::out);
    file << "cjnbwuhvcwirohfwpo\n";
    file.close();
    int response = os::compress_file(logFile);
    ASSERT_EQ(response,SUCCESS);
    std::filesystem::remove_all(TEMP_WORK_DIR);
}

TEST_F(OS_TEST, CompressFileWithInvaliFilePathStringInput)
{
    int result = os::create_dir(TEMP_WORK_DIR);
    ASSERT_EQ(result, SUCCESS);
    const string &logFile = "/home/champ/Desktop/sample/smple.txt";
    int response = os::compress_file(logFile);
    EXPECT_EQ(response,FAILED);
}

TEST_F(OS_TEST, CompressDirectoryWithvaliFilePathStringInput)
{
    const string &logFile = "/home/champ/Downloads/Pravin";
    int response = os::compress_file(logFile);
    EXPECT_EQ(response,FAILED);
}

TEST_F(OS_TEST, GetRegularFilesWithCorrectDirectoryPath)
{
    const string &directory = "/home/champ/Desktop/scm";
    vector<string> files;
    int response = os::get_regular_files(directory,files);
    ASSERT_EQ(response,SUCCESS);
    for (string f: files) {std::cout << f << '\n';}
}

TEST_F(OS_TEST, GetRegularFilesWithCorrectDirectoryPathWithTrailingAndEndingWhiteSpace)
{
    const string &directory = "  /home/champ/Desktop/scm ";
    vector<string> files;
    int response = os::get_regular_files(directory,files);
    ASSERT_EQ(response,SUCCESS);
    for (string f: files) {std::cout << f << '\n';}
}

TEST_F(OS_TEST, GetRegularFilesWithEmptyStringDirectoryPath)
{
    const string &directory = "";
    vector<string> files;
    int response = os::get_regular_files(directory,files);
    ASSERT_EQ(response,FAILED);
    for (string f: files) {std::cout << f << '\n';}
}

TEST_F(OS_TEST, GetRegularFilesWithInCorrectDirectoryPath)
{
    const string &directory = "/home/champ/Desktop/cm";
    vector<string> files;
    int response = os::get_regular_files(directory,files);
    ASSERT_EQ(response,FAILED);
    for (string f: files) {std::cout << f << '\n';}
}


TEST_F(OS_TEST, GetArchiveFilePathByCurrentDayAndAppNameWithValidDay)
{
    const string &type = "syslog";
    string file_path;
    int result = os::create_log_archive_file(os::current_day, os::current_month, os::current_year, file_path, type);
    ASSERT_EQ(result, SUCCESS);
    string expected_result = "/etc/scl/log/archives/2024/Apr/" + std::to_string(os::current_day) + "-" + type;
    string response = os::get_archive_by_day_and_log_name(os::current_day, type);
    EXPECT_STREQ(response.c_str(), expected_result.c_str());
}

TEST_F(OS_TEST, GetArchiveFilePathByCurrentDayAndAppNameWithDay0)
{
    const string &type = "syslog";
    string response = os::get_archive_by_day_and_log_name(0, type);
    EXPECT_STREQ(response.c_str(), "");
}
TEST_F(OS_TEST, GetArchiveFilePathByCurrentDayAndAppNameWithDay32)
{
    const string &type = "syslog";
    string response = os::get_archive_by_day_and_log_name(32, type);
    EXPECT_STREQ(response.c_str(), "");
}


TEST_F(OS_TEST, CreateJsonFilePathForLogTypeSyslog)
{
    const string &type = "syslog";
    string expected_result = "/etc/scl/log/json/" + common::trim(type) + "/"+os::get_current_time_with_no_space() + ".json";
    string response = os::create_json_file(type);
    ASSERT_STREQ(response.c_str(), expected_result.c_str());
}

TEST_F(OS_TEST, CreateJsonFilePathForLogTypedpkg)
{
    const string &type = " dpkg";
    string expected_result = "/etc/scl/log/json/" + common::trim(type) + "/"+os::get_current_time_with_no_space() + ".json";
    string response = os::create_json_file(type);
    ASSERT_STREQ(response.c_str(), expected_result.c_str());
}

TEST_F(OS_TEST, CreateJsonFilePathForEmptyStringLogType)
{
    const string &type = "";
    string response = os::create_json_file(type);
    ASSERT_STREQ(response.c_str(), "");
}