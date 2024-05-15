// #include <gmock/gmock.h>
// using ::testing::AtLeast;
// using ::testing::Return;
// using ::testing::_;
// using ::testing::Invoke;
// using ::testing::DoDefault;

// class osMock {
// public:
//     static int current_day;
//     static int current_month;
//     static int current_year;
//     static std::string host_name;

//     static bool isDirExist(const std::string& dir_name);
//     static bool isFileExist(const std::string& file);
//     static int createDir(const std::string& dir_name);
//     static int createFile(const std::string& file_path);
//     static int deleteFile(const std::string& file_name);
//     static int createLogArchiveFile(int c_day, int c_month, int c_year, std::string& file_path, const std::string& app_name);
//     static int handleLocalLogFile(int day, int month, int year, std::string& file_path, const std::string& app_name);
//     static std::string getPathOrBackupFilePath(std::string filename);
//     static int compressFile(const std::string& log_file);
//     static std::string getArchiveByDayAndLogName(const std::string& app_name);
//     static int getRegularFiles(const std::string& directory, std::vector<std::string>& files);
//     static std::string createJSONFile(const std::string& type);
//     static std::string sign(const std::string& file, const std::string& sign_key);
//     static bool verifySignature(const std::string& file, const std::string& sign_key, const std::string& signed_data);
//     static std::string getCurrentTimeWithNoSpace();
//     static std::string getCurrentTime();
//     static bool validatePath(const std::string& file_path);
// };

// class Mockos : public osMock {
// public:
//     MOCK_METHOD(bool, isDirExist, (const std::string& dir_name), ());
//     MOCK_METHOD(bool, isFileExist, (const std::string& file), ());
//     MOCK_METHOD(int, createDir, (const std::string& dir_name), ());
//     MOCK_METHOD(int, createFile, (const std::string& file_path), ());
//     MOCK_METHOD(int, deleteFile, (const std::string& file_name), ());
//     MOCK_METHOD(int, createLogArchiveFile, (int c_day, int c_month, int c_year, std::string& file_path, const std::string& app_name), ());
//     MOCK_METHOD(int, handleLocalLogFile, (int day, int month, int year, std::string& file_path, const std::string& app_name), ());
//     MOCK_METHOD(std::string, getPathOrBackupFilePath, (std::string filename), ());
//     MOCK_METHOD(int, compressFile, (const std::string& log_file), ());
//     MOCK_METHOD(std::string, getArchiveByDayAndLogName, (const std::string& app_name), ());
//     MOCK_METHOD(int, getRegularFiles, (const std::string& directory, std::vector<std::string>& files), ());
//     MOCK_METHOD(std::string, createJSONFile, (const std::string& type), ());
//     MOCK_METHOD(std::string, sign, (const std::string& file, const std::string& sign_key), ());
//     MOCK_METHOD(bool, verifySignature, (const std::string& file, const std::string& sign_key, const std::string& signed_data), ());
//     MOCK_METHOD(std::string, getCurrentTimeWithNoSpace, (), ());
//     MOCK_METHOD(std::string, getCurrentTime, (), ());
//     MOCK_METHOD(bool, validatePath, (const std::string& file_path), ());
// };

