#include "os.hpp"

int os::current_day = 0;
int os::current_month = 0;
int os::current_year = 0;
string os::host_name = "platform";

int os::compress_file(const string &log_file)
{
	DEBUG("Backup started for log_file: ", log_file, " size: ", std::to_string(os::get_file_size(log_file)));
	int result = Audit::SUCCESS;
	string line;
	string new_file_path = log_file;
	if (!os::is_file_exist(log_file) || std::filesystem::is_directory(log_file))
	{
		LOG_ERROR("invalid filepath or directoy given" + log_file);
		return Audit::FAILED;
	}
	{
		vector<string> files;
		os::get_regular_files(log_file.substr(0, log_file.find_last_of('/')), files);
		new_file_path += +"." + std::to_string(files.size());
	}

	fstream file(log_file, std::ios::in | std::ios::binary);
	if (!file.is_open())
	{
		LOG_ERROR("Permission denied to read : ", log_file);
		return Audit::FAILED;
	}
#if __linux
	gzFile z_log;
	string zip_file = new_file_path + ".gz";
	z_log = gzopen(zip_file.c_str(), "w");
	if (!z_log)
	{
		LOG_ERROR(Audit::FCREATION_FAILED , zip_file);
		file.close();
		return Audit::FAILED;
	}

	while (std::getline(file, line))
	{
		if (line.size() == 0)
			continue;
		if (gzwrite(z_log, line.c_str(), static_cast<unsigned int>(line.size())) != (int)line.size())
		{
			LOG_ERROR(Audit::FWRITE_FAILED + zip_file);
			result = Audit::FAILED;
			break;
		}
	}
	file.close();
	gzclose(z_log);
	if (result == Audit::SUCCESS)
	{
		os::delete_file(new_file_path);
	}
	else
	{
		LOG_ERROR(Audit::FDELETE_FAILED + new_file_path);
	}
#else
	DEBUG("archive not supported fot this platform");
#endif
	return result;
}

string os::sign(const string &file, const string &sign_key)
{
	/*if (!os::is_file_exist(file))
	{
		LOG_ERROR(Audit::FILE_ERROR, file);
		return "";
	}*/
	// Redundant check
#if __linux__
	std::ifstream file_data(file, std::ios::binary);
	if (!file_data.is_open())
	{
		LOG_ERROR(Audit::FILE_ERROR, file);
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

bool os::verify_signature(const string &file, const string &sign_key, const string &signed_data)
{
	string hash = sign(file, sign_key);

	return (hash == signed_data) ? true : false;
}

bool os::is_file_exist(const string &file)
{
	return std::filesystem::exists(common::trim(file));
}

string os::get_path_or_backup_file_path(const string &filename)
{
	string non_empty_path;
	if (is_file_exist(filename))
	{
		auto file_size = os::get_file_size(filename);

		non_empty_path = (file_size > 0L) ? filename : filename + ".1";
	}
	else
	{
		non_empty_path = "";
	}
	return non_empty_path;
}

string os::get_archive_by_day_and_log_name(const int day, const string &log_name)
{
	if (day <= 0 || day > 31)
	{
		LOG_ERROR("Day value should in between (1-31)");
		return "";
	}
	string current_log_file = Audit::Config::BASE_LOG_DIR;
	current_log_file += Audit::Config::BASE_LOG_ARCHIVE;
	current_log_file += Audit::Config::SEP + std::to_string(current_year) + Audit::Config::SEP + Audit::MONTHS[current_month] + Audit::Config::SEP + std::to_string(day) + "-" + log_name;
	return os::is_file_exist(current_log_file) ? current_log_file : "";
}

int os::handle_local_log_file(int day, int month, int year, string &file_path, const string &app_name)
{
	int result = Audit::SUCCESS;
	string current_dir = Audit::Config::BASE_LOG_DIR;
	if (day != current_day)
	{
		string current_log_file = get_archive_by_day_and_log_name(current_day, app_name);

		if ((result = compress_file(current_log_file)) == Audit::FAILED)
		{
			return Audit::FAILED;
		}
	}
	result = create_log_archive_file(day, month, year, file_path, app_name);
	return result;
}

int os::create_log_archive_file(int curr_day, int curr_month, int curr_year, string &file_path, const string &app_name)
{
	if (common::trim(app_name).empty())
	{
		return Audit::FAILED;
	}
	string current_dir = Audit::Config::BASE_LOG_DIR;
	if (!is_dir_exist(current_dir) && create_dir(current_dir) == Audit::SUCCESS)
	{
		LOG("path created", current_dir);
	}
	else if (!is_dir_exist(current_dir) && create_dir(current_dir) != Audit::SUCCESS)
	{
		LOG_ERROR("Check user permission to create file at this location: " + current_dir);
		return Audit::FAILED;
	}
	current_dir += Audit::Config::BASE_LOG_ARCHIVE;
	if (!is_dir_exist(current_dir) && create_dir(current_dir) == Audit::SUCCESS)
	{
		LOG("path created", current_dir);
	}
	else if (!is_dir_exist(current_dir) && create_dir(current_dir) != Audit::SUCCESS)
	{
		LOG_ERROR("Check user permission to create file at this location: " + current_dir);
		return Audit::FAILED;
	}
	current_dir += Audit::Config::SEP + std::to_string(curr_year);
	if (!is_dir_exist(current_dir) && create_dir(current_dir) == Audit::SUCCESS)
	{
		LOG("path created", current_dir);
	}
	else if (!is_dir_exist(current_dir) && create_dir(current_dir) != Audit::SUCCESS)
	{
		LOG_ERROR("Check user permission to create file at this location: " + current_dir);
		return Audit::FAILED;
	}
	int inclusive_month = curr_month;
	current_dir += Audit::Config::SEP + Audit::MONTHS[inclusive_month];
	if (!is_dir_exist(current_dir) && create_dir(current_dir) == Audit::SUCCESS)
	{
		LOG("path created", current_dir);
	}
	else if (!is_dir_exist(current_dir) && create_dir(current_dir) != Audit::SUCCESS)
	{
		LOG_ERROR("Check user permission to create file at this location: " + current_dir);
		return Audit::FAILED;
	}
	current_dir += Audit::Config::SEP + std::to_string(curr_day) + "-" + app_name;
	file_path = current_dir;
	if (os::is_file_exist(file_path))
	{
		return Audit::SUCCESS;
	}
	fstream new_file(file_path, std::ios::out);
	if (new_file)
	{
		new_file.close();
		current_day = curr_day;
		current_month = curr_month;
		current_year = curr_year;
		return Audit::SUCCESS;
	}
	else
	{
		LOG_ERROR(Audit::FCREATION_FAILED, file_path);
	}
	return Audit::FAILED;
}

int os::delete_file(const string &file_name)
{
	string path = common::trim(file_name);
	if (is_file_exist(path))
	{
		try
		{
			if (std::filesystem::remove(path))
			{
				return Audit::SUCCESS;
			}
		}
		catch (const std::exception &e)
		{
			string error(e.what());
			LOG_ERROR(error);
		}
	}
	return Audit::FAILED;
}

int os::create_dir(const string &dir_name)
{
	string directory = common::trim(dir_name);
	if (is_dir_exist(directory) && !directory.empty())
	{
		return Audit::SUCCESS;
	}
	try
	{
		if (std::filesystem::create_directory(directory) && !directory.empty())
			return Audit::SUCCESS;
	}
	catch (const std::exception &e)
	{
		DEBUG(e.what());
	}
	return Audit::FAILED;
}

int os::create_file(const string &file_path)
{
	string path = common::trim(file_path);
	try
	{
		if (is_file_exist(path)) // Precheck if file already exists
		{
			return Audit::SUCCESS;
		}
		if (!validate_path(path))
		{
			return Audit::FAILED;
		}
		std::ofstream file(path);
		if (file.is_open())
		{
			DEBUG("New file createad at: " + path);
			file.close();
			return Audit::SUCCESS;
		}
	}
	catch (const std::exception &e)
	{
		LOG_ERROR(e.what());
	}
	return Audit::FAILED;
}

bool os::is_dir_exist(const string &dir_name)
{
	return std::filesystem::exists(common::trim(dir_name));
}

int os::get_regular_files(const string &directory, vector<string> &files)
{
	int result = Audit::SUCCESS;
	try
	{
		for (const auto &entry : std::filesystem::directory_iterator(common::trim(directory)))
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
		result = Audit::FAILED;
		LOG_ERROR(e.what());
	}
	return result;
}

std::string os::get_current_time()
{
	std::ostringstream oss;
	time_t now;
	time(&now);

	struct tm *timeinfo = gmtime(&now); // Get UTC time

	// Format time as string
	oss << std::put_time(timeinfo, "%Y-%m-%dT%H:%M:%S.") << std::setfill('0') << std::setw(7) << (now % 10000000) << "+00:00"; // Append microseconds and timezone offset

	return oss.str();
}

string os::get_current_time_with_no_space()
{
	string time = os::get_current_time();
	auto mid = time.find_first_of(' ');
	return time.substr(0, mid) + "_" + time.substr(mid + 1);
}

string os::create_json_file(const string &type)
{
	if (common::trim(type).empty())
	{
		LOG_ERROR("Log type should not be empty");
		return "";
	}

	string file_path = Audit::Config::BASE_LOG_DIR;

	if (!os::is_dir_exist(file_path) && os::create_dir(file_path) != Audit::SUCCESS)
	{
		return "";
	}
	file_path += "json";
	if (!os::is_dir_exist(file_path) && os::create_dir(file_path) != Audit::SUCCESS)
	{
		return "";
	}
	file_path += Audit::Config::SEP + common::trim(type);
	if (!os::is_dir_exist(file_path) && os::create_dir(file_path) != Audit::SUCCESS)
	{
		return "";
	}
	file_path += Audit::Config::SEP + os::get_current_time_with_no_space() + ".json";
	std::ofstream file(file_path);
	if (!file)
	{
		LOG_ERROR(Audit::FILE_ERROR + file_path);
		return "";
	}
	file.close();
	return file_path;
}

bool os::validate_path(const string &file_path)
{
	if (common::trim(file_path).empty())
	{
		LOG_ERROR("Given Path is empty to create file.");
		return false;
	}
	auto pos = file_path.find_last_of(Audit::Config::SEP);
	return (!is_file_exist(file_path.substr(0, pos)) && create_dir(file_path.substr(0, pos)) != Audit::SUCCESS) ? false : true;
}

string os::read_json_file(const string &json_file_path)
{
	if (!is_file_exist(json_file_path)) { return "" ;}
	std::ifstream jsonFile(json_file_path);
	if (!jsonFile.is_open())
	{
		LOG_ERROR(Audit::FILE_ERROR, json_file_path);
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

std::uintmax_t os::get_file_size(const string &file_name)
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
