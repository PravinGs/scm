#include "os.hpp"

int os::current_day = 0;
int os::current_month = 0;
int os::current_year = 0;
string os::host_name = "platform";

int os::compressFile(const string &log_file)
{
	DEBUG("Backup started for log_file: ", log_file, " size: ", std::to_string(os::getFileSize(log_file)));
	int result = SCM::SUCCESS;
	string line;
	string new_file_path = log_file;
	if (!os::isFileExist(log_file) || std::filesystem::is_directory(log_file))
	{
		LOG_ERROR("invalid filepath or directoy given" + log_file);
		return SCM::FAILED;
	}
	{
		vector<string> files;
		os::getRegularFiles(log_file.substr(0, log_file.find_last_of('/')), files);
		new_file_path += +"." + std::to_string(files.size());
	}

	fstream file(log_file, std::ios::in | std::ios::binary);
	if (!file.is_open())
	{
		LOG_ERROR("Permission denied to read : ", log_file);
		return SCM::FAILED;
	}
#if __linux
	gzFile z_log;
	string zip_file = new_file_path + ".gz";
	z_log = gzopen(zip_file.c_str(), "w");
	if (!z_log)
	{
		LOG_ERROR(SCM::FCREATION_FAILED , zip_file);
		file.close();
		return SCM::FAILED;
	}

	while (std::getline(file, line))
	{
		if (line.size() == 0)
			continue;
		if (gzwrite(z_log, line.c_str(), static_cast<unsigned int>(line.size())) != (int)line.size())
		{
			LOG_ERROR(SCM::FWRITE_FAILED + zip_file);
			result = SCM::FAILED;
			break;
		}
	}
	file.close();
	gzclose(z_log);
	if (result == SCM::SUCCESS)
	{
		os::deleteFile(new_file_path);
	}
	else
	{
		LOG_ERROR(SCM::FDELETE_FAILED + new_file_path);
	}
#else
	DEBUG("archive not supported fot this platform");
#endif
	return result;
}

string os::sign(const string &file, const string &sign_key)
{
	/*if (!os::isFileExist(file))
	{
		LOG_ERROR(SCM::FILE_ERROR, file);
		return "";
	}*/
	// Redundant check
#if __linux__
	std::ifstream file_data(file, std::ios::binary);
	if (!file_data.is_open())
	{
		LOG_ERROR(SCM::FILE_ERROR, file);
		return "";
	}
	string data((std::istreambuf_iterator<char>(file_data)), std::istreambuf_iterator<char>());

	unsigned char hash[EVP_MAX_MD_SIZE];
	unsigned int hash_length;

	HMAC_CTX *hmac_ctx = HMAC_CTX_new();
	HMAC_Init_ex(hmac_ctx, sign_key.c_str(), sign_key.length(), EVP_sha256(), nullptr);
	HMAC_Update(hmac_ctx, reinterpret_cast<const unsigned char *>(data.c_str()), data.length());
	HMAC_Final(hmac_ctx, hash, &hash_length);
	HMAC_CTX_free(hmac_ctx);

	return string(reinterpret_cast<char *>(hash), hash_length);
#else
	DEBUG("Crypto feature not supported in this platform");
	return "";
#endif
}

bool os::verifySignature(const string &file, const string &sign_key, const string &signed_data)
{
	string hash = sign(file, sign_key);

	return (hash == signed_data) ? true : false;
}

bool os::isFileExist(const string &file)
{
	return std::filesystem::exists(Common::trim(file));
}

string os::getPathOrBackupFilePath(const string &filename)
{
	string non_empty_path;
	if (isFileExist(filename))
	{
		auto file_size = os::getFileSize(filename);

		non_empty_path = (file_size > 0L) ? filename : filename + ".1";
	}
	else
	{
		non_empty_path = "";
	}
	return non_empty_path;
}

string os::getArchiveByDayAndLogName(const int day, const string &log_name)
{
	if (day <= 0 || day > 31)
	{
		LOG_ERROR("Day value should in between (1-31)");
		return "";
	}
	string current_log_file = SCM::Config::BASE_LOG_DIR;
	current_log_file += SCM::Config::BASE_LOG_ARCHIVE;
	current_log_file += SCM::Config::SEP + std::to_string(current_year) + SCM::Config::SEP + SCM::MONTHS[current_month] + SCM::Config::SEP + std::to_string(day) + "-" + log_name;
	return os::isFileExist(current_log_file) ? current_log_file : "";
}

int os::handleLocalLogFile(int day, int month, int year, string &file_path, const string &app_name)
{
	int result = SCM::SUCCESS;
	string current_dir = SCM::Config::BASE_LOG_DIR;
	if (day != current_day)
	{
		string current_log_file = getArchiveByDayAndLogName(current_day, app_name);

		if ((result = compressFile(current_log_file)) == SCM::FAILED)
		{
			return SCM::FAILED;
		}
	}
	result = createLogArchiveFile(day, month, year, file_path, app_name);
	return result;
}

int os::createLogArchiveFile(int curr_day, int curr_month, int curr_year, string &file_path, const string &app_name)
{
	if (Common::trim(app_name).empty())
	{
		return SCM::FAILED;
	}
	string current_dir = SCM::Config::BASE_LOG_DIR;
	if (!isDirExist(current_dir) && createDir(current_dir) == SCM::SUCCESS)
	{
		LOG("path created", current_dir);
	}
	else if (!isDirExist(current_dir) && createDir(current_dir) != SCM::SUCCESS)
	{
		LOG_ERROR("Check user permission to create file at this location: " + current_dir);
		return SCM::FAILED;
	}
	current_dir += SCM::Config::BASE_LOG_ARCHIVE;
	if (!isDirExist(current_dir) && createDir(current_dir) == SCM::SUCCESS)
	{
		LOG("path created", current_dir);
	}
	else if (!isDirExist(current_dir) && createDir(current_dir) != SCM::SUCCESS)
	{
		LOG_ERROR("Check user permission to create file at this location: " + current_dir);
		return SCM::FAILED;
	}
	current_dir += SCM::Config::SEP + std::to_string(curr_year);
	if (!isDirExist(current_dir) && createDir(current_dir) == SCM::SUCCESS)
	{
		LOG("path created", current_dir);
	}
	else if (!isDirExist(current_dir) && createDir(current_dir) != SCM::SUCCESS)
	{
		LOG_ERROR("Check user permission to create file at this location: " + current_dir);
		return SCM::FAILED;
	}
	int inclusive_month = curr_month;
	current_dir += SCM::Config::SEP + SCM::MONTHS[inclusive_month];
	if (!isDirExist(current_dir) && createDir(current_dir) == SCM::SUCCESS)
	{
		LOG("path created", current_dir);
	}
	else if (!isDirExist(current_dir) && createDir(current_dir) != SCM::SUCCESS)
	{
		LOG_ERROR("Check user permission to create file at this location: " + current_dir);
		return SCM::FAILED;
	}
	current_dir += SCM::Config::SEP + std::to_string(curr_day) + "-" + app_name;
	file_path = current_dir;
	if (os::isFileExist(file_path))
	{
		return SCM::SUCCESS;
	}
	fstream new_file(file_path, std::ios::out);
	if (new_file)
	{
		new_file.close();
		current_day = curr_day;
		current_month = curr_month;
		current_year = curr_year;
		return SCM::SUCCESS;
	}
	else
	{
		LOG_ERROR(SCM::FCREATION_FAILED, file_path);
	}
	return SCM::FAILED;
}

int os::deleteFile(const string &file_name)
{
	string path = Common::trim(file_name);
	if (isFileExist(path))
	{
		try
		{
			if (std::filesystem::remove(path))
			{
				return SCM::SUCCESS;
			}
		}
		catch (const std::exception &e)
		{
			string error(e.what());
			LOG_ERROR(error);
		}
	}
	return SCM::FAILED;
}

int os::createDir(const string &dir_name)
{
	string directory = Common::trim(dir_name);
	if (isDirExist(directory) && !directory.empty())
	{
		return SCM::SUCCESS;
	}
	try
	{
		if (std::filesystem::create_directory(directory) && !directory.empty())
			return SCM::SUCCESS;
	}
	catch (const std::exception &e)
	{
		DEBUG(e.what());
	}
	return SCM::FAILED;
}

int os::createFile(const string &file_path)
{
	string path = Common::trim(file_path);
	try
	{
		if (isFileExist(path)) // Precheck if file already exists
		{
			return SCM::SUCCESS;
		}
		if (!validatePath(path))
		{
			return SCM::FAILED;
		}
		std::ofstream file(path);
		if (file.is_open())
		{
			DEBUG("New file createad at: " + path);
			file.close();
			return SCM::SUCCESS;
		}
	}
	catch (const std::exception &e)
	{
		LOG_ERROR(e.what());
	}
	return SCM::FAILED;
}

bool os::isDirExist(const string &dir_name)
{
	return std::filesystem::exists(Common::trim(dir_name));
}

int os::getRegularFiles(const string &directory, vector<string> &files)
{
	int result = SCM::SUCCESS;
	try
	{
		for (const auto &entry : std::filesystem::directory_iterator(Common::trim(directory)))
		{
			if (std::filesystem::is_regular_file(entry.path()))
			{
				string child = entry.path().string();
				files.push_back(child);
			}
		}
	}
	catch (exception &e)
	{
		result = SCM::FAILED;
		LOG_ERROR(e.what());
	}
	return result;
}

std::string os::getCurrentTime()
{
	std::ostringstream oss;
	time_t now;
	time(&now);

	struct tm *timeinfo = gmtime(&now); // Get UTC time

	// Format time as string
	oss << std::put_time(timeinfo, "%Y-%m-%dT%H:%M:%S.") << std::setfill('0') << std::setw(7) << (now % 10000000) << "+00:00"; // Append microseconds and timezone offset

	return oss.str();
}

string os::getCurrentTimeWithNoSpace()
{
	string time = os::getCurrentTime();
	auto mid = time.find_first_of(' ');
	return time.substr(0, mid) + "_" + time.substr(mid + 1);
}

string os::createJSONFile(const string &type)
{
	if (Common::trim(type).empty())
	{
		LOG_ERROR("Log type should not be empty");
		return "";
	}

	string file_path = SCM::Config::BASE_LOG_DIR;

	if (!os::isDirExist(file_path) && os::createDir(file_path) != SCM::SUCCESS)
	{
		return "";
	}
	file_path += "json";
	if (!os::isDirExist(file_path) && os::createDir(file_path) != SCM::SUCCESS)
	{
		return "";
	}
	file_path += SCM::Config::SEP + Common::trim(type);
	if (!os::isDirExist(file_path) && os::createDir(file_path) != SCM::SUCCESS)
	{
		return "";
	}
	file_path += SCM::Config::SEP + os::getCurrentTimeWithNoSpace() + ".json";
	std::ofstream file(file_path);
	if (!file)
	{
		LOG_ERROR(SCM::FILE_ERROR + file_path);
		return "";
	}
	file.close();
	return file_path;
}

bool os::validatePath(const string &file_path)
{
	if (Common::trim(file_path).empty())
	{
		LOG_ERROR("Given Path is empty to create file.");
		return false;
	}
	auto pos = file_path.find_last_of(SCM::Config::SEP);
	return (!isFileExist(file_path.substr(0, pos)) && createDir(file_path.substr(0, pos)) != SCM::SUCCESS) ? false : true;
}

string os::readJSONFile(const string &json_file_path)
{
	if (!isFileExist(json_file_path)) { return "" ;}
	std::ifstream jsonFile(json_file_path);
	if (!jsonFile.is_open())
	{
		LOG_ERROR(SCM::FILE_ERROR, json_file_path);
		return "";
	}
	Json::Value root; 
	Json::CharReaderBuilder builder;
	std::string errs;
	if (!parseFromStream(builder, jsonFile, &root, &errs))
	{
		LOG_ERROR("Failed to parse JSON file ", json_file_path, errs);
		return "";
	}
	Json::StreamWriterBuilder writer_builder;
	return Json::writeString(writer_builder, root);
}

std::uintmax_t os::getFileSize(const string &file_name)
{
    std::uintmax_t size;
    try
    {
        size = std::filesystem::file_size(file_name);
    }
    catch (std::exception &ex)
    {
        DEBUG(ex.what());
        std::cerr << ex.what() << '\n';
    }

    return size;
}
