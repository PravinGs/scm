#ifndef OS_HPP
#define OS_HPP
#pragma once 
#include "common.hpp"
class os
{
public:
    static int current_day;
    static int current_month;
    static int current_year;
    static string host_name;
 
    static bool is_dir_exist(const string& dir_name);
    static bool is_file_exist(const string& file);
    static int create_dir(const string& dir_name);
    static int create_file(const string& file_path);
    static int delete_file(const string& file_name);
    static int create_log_archive_file(int c_day, int c_month, int c_year, string& file_path, const string& app_name);
    // Mock required
    static int handle_local_log_file(int day, int month, int year, string& file_path, const string& app_name);
    static string get_path_or_backup_file_path(const string& filename);
    static int compress_file(const string& log_file);
    static string get_archive_by_day_and_log_name(const int day, const string& log_name);
    static int get_regular_files(const string& directory, vector<string>& files);

    static string create_json_file(const string& type);
    static string sign(const string& file, const string& sign_key);
    static bool verify_signature(const string& file, const string& sign_key, const string& signed_data);
    static string get_current_time_with_no_space();
    static string get_current_time();
    static bool validate_path(const string& file_path);

    static string read_json_file(const string& json_file_path);
    static std::uintmax_t get_file_size(const string& file_path);
};
#endif //OS_HPP