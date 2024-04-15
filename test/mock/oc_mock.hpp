// #include <gmock/gmock.h>
// using ::testing::AtLeast;
// using ::testing::Return;
// using ::testing::_;
// using ::testing::Invoke;
// using ::testing::DoDefault;

// class OsMock {
// public:
//     static int current_day;
//     static int current_month;
//     static int current_year;
//     static std::string host_name;

//     static bool is_dir_exist(const std::string& dir_name);
//     static bool is_file_exist(const std::string& file);
//     static int create_dir(const std::string& dir_name);
//     static int create_file(const std::string& file_path);
//     static int delete_file(const std::string& file_name);
//     static int create_log_archive_file(int c_day, int c_month, int c_year, std::string& file_path, const std::string& app_name);
//     static int handle_local_log_file(int day, int month, int year, std::string& file_path, const std::string& app_name);
//     static std::string get_path_or_backup_file_path(std::string filename);
//     static int compress_file(const std::string& log_file);
//     static std::string get_archive_by_day_and_log_name(const std::string& app_name);
//     static int get_regular_files(const std::string& directory, std::vector<std::string>& files);
//     static std::string create_json_file(const std::string& type);
//     static std::string sign(const std::string& file, const std::string& sign_key);
//     static bool verify_signature(const std::string& file, const std::string& sign_key, const std::string& signed_data);
//     static std::string get_current_time_with_no_space();
//     static std::string get_current_time();
//     static bool validate_path(const std::string& file_path);
// };

// class MockOs : public OsMock {
// public:
//     MOCK_METHOD(bool, is_dir_exist, (const std::string& dir_name), ());
//     MOCK_METHOD(bool, is_file_exist, (const std::string& file), ());
//     MOCK_METHOD(int, create_dir, (const std::string& dir_name), ());
//     MOCK_METHOD(int, create_file, (const std::string& file_path), ());
//     MOCK_METHOD(int, delete_file, (const std::string& file_name), ());
//     MOCK_METHOD(int, create_log_archive_file, (int c_day, int c_month, int c_year, std::string& file_path, const std::string& app_name), ());
//     MOCK_METHOD(int, handle_local_log_file, (int day, int month, int year, std::string& file_path, const std::string& app_name), ());
//     MOCK_METHOD(std::string, get_path_or_backup_file_path, (std::string filename), ());
//     MOCK_METHOD(int, compress_file, (const std::string& log_file), ());
//     MOCK_METHOD(std::string, get_archive_by_day_and_log_name, (const std::string& app_name), ());
//     MOCK_METHOD(int, get_regular_files, (const std::string& directory, std::vector<std::string>& files), ());
//     MOCK_METHOD(std::string, create_json_file, (const std::string& type), ());
//     MOCK_METHOD(std::string, sign, (const std::string& file, const std::string& sign_key), ());
//     MOCK_METHOD(bool, verify_signature, (const std::string& file, const std::string& sign_key, const std::string& signed_data), ());
//     MOCK_METHOD(std::string, get_current_time_with_no_space, (), ());
//     MOCK_METHOD(std::string, get_current_time, (), ());
//     MOCK_METHOD(bool, validate_path, (const std::string& file_path), ());
// };

