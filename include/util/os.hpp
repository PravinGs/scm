#ifndef os_HPP
#define os_HPP
#pragma once 
#include "Common.hpp"
class os
{
public:
    static int current_day;
    static int current_month;
    static int current_year;
    static string host_name;
 
    static bool isDirExist(const string& dir_name);
    static bool isFileExist(const string& file);
    static int createDir(const string& dir_name);
    static int createFile(const string& file_path);
    static int deleteFile(const string& file_name);
    static int createLogArchiveFile(int c_day, int c_month, int c_year, string& file_path, const string& app_name);
    // Mock required
    static int handleLocalLogFile(int day, int month, int year, string& file_path, const string& app_name);
    static string getPathOrBackupFilePath(const string& filename);
    static int compressFile(const string& log_file);
    static string getArchiveByDayAndLogName(const int day, const string& log_name);
    static int getRegularFiles(const string& directory, vector<string>& files);

    static string createJSONFile(const string& type);
    static string sign(const string& file, const string& sign_key);
    static bool verifySignature(const string& file, const string& sign_key, const string& signed_data);
    static string getCurrentTime();
    static bool validatePath(const string& file_path);

    static string readJSONFile(const string& json_file_path);
    static std::uintmax_t getFileSize(const string& file_path);
};
#endif //os_HPP