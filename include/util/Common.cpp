#include "Common.hpp"

fstream Common::logfp;
std::mutex logfile_mutex;
bool Common::debug = false;

bool debug_enabled = false;
bool console_log_enabled = false;

string accessToken = "";

void write_to_file(const string &log)
{
    Common::writeLog(log);
}

int Common::setupLogger(const string &log_file)
{
    int result = SCM::SUCCESS;
#if __linux__
    if (os::isFileExist(SCM::Config::SYSLOG_FILE))
    {
        SCM::is_syslog_enabled = true;
        return SCM::SUCCESS;
    }
    /*std::uintmax_t s1 = getFileSize(SCM::Config::SYSLOG_FILE);
    if (s1 > 0L)
    {
        SCM::is_syslog_enabled = true;
        return SCM::SUCCESS;
    }
    else if (s1 == 0L)
    {
        syslog(LOG_INFO, "Syslog probe message for testing");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::uintmax_t s2 = getFileSize(SCM::Config::SYSLOG_FILE);
        if (s2 > s1)
        {
            SCM::is_syslog_enabled = true;
        }
        else
        {
        }
    }*/
#endif
    if (!SCM::is_syslog_enabled && !os::isFileExist(log_file))
    {
        result = os::createFile(log_file);
    }
    return result;
}

int Common::backupLogFile(const string &file_path)
{
    if (os::isFileExist(file_path) && isFileGreaterThanMaxSize(file_path) && os::compressFile(file_path) == SCM::SUCCESS)
    {
        Common::logfp.close();
        Common::logfp.open(file_path, std::ios::out);
        return SCM::SUCCESS;
    }
    else
    {
        return SCM::FAILED;
    }
}

bool Common::isFileGreaterThanMaxSize(const string &log_file)
{
    std::uintmax_t file_size = os::getFileSize(log_file);
    double max_file_size = static_cast<double>(file_size) / (1024 * 1024);
    return max_file_size > 5.0;
}

string Common::toLowerCase(string &str)
{
    string lower_case_string;
    for (char ch : str)
    {
        lower_case_string += std::tolower(ch);
    }
    return lower_case_string;
}

bool Common::isValidSysLogTimeString(const string &s_time)
{
    std::tm tm_struct = {};
    return (strptime(s_time.c_str(), "%b %e %H:%M:%S", &tm_struct) != nullptr) ? true : false;
}

string Common::createLogCacheFile(const string &app_name)
{
    if (app_name.empty())
    {
        LOG_ERROR("App name should not be empty to create a cache file.");
        return "";
    }
    string file_path = SCM::Config::BASE_CONFIG_DIR;
    if (!os::isDirExist(file_path) && os::createDir(file_path) != SCM::SUCCESS)
    {
        return "";
    }
    file_path += SCM::Config::BASE_CONFIG_TMP;
    if (!os::isDirExist(file_path) && os::createDir(file_path) != SCM::SUCCESS)
    {
        return "";
    }
    file_path += app_name;
    if (!os::isFileExist(file_path) && os::createFile(file_path) != SCM::SUCCESS)
    {
        return "";
    }
    return file_path;
}

int Common::updateLogWrittenTime(const string &app_name, const string &time)
{
    if (app_name.empty() || time.empty())
    {
        LOG_ERROR((app_name.empty() ? "Appname shouldn't be empty" : app_name), (time.empty() ? "Time shouldn't be empty" : time));
        return SCM::FAILED;
    }
    string file_path = createLogCacheFile(app_name);
    fstream file(file_path, std::ios::out);
    if (!file.is_open())
    {
        LOG_ERROR(SCM::INVALID_FILE, file_path);
        return SCM::FAILED;
    }
    file << time;
    INFO("time ", time, " updated to ", file_path);
    file.close();
    return SCM::SUCCESS;
}

string Common::trim(string line)
{
    const auto str_begin = line.find_first_not_of(" \t");
    if (str_begin == string::npos)
        return "";

    const auto strEnd = line.find_last_not_of(" \t");
    const auto strRange = strEnd - str_begin + 1;

    string str = line.substr(str_begin, strRange);
    return (str.length() >= 2 && str[0] == '"' && str[str.length() - 1] == '"') ? str.substr(1, str.length() - 2) : str;
}

string Common::getHostName()
{
    string host;
#if __linux__
    char hostname[256];

    if (gethostname(hostname, sizeof(hostname)) == 0)
    {
        hostname[strlen(hostname)] = '\0';
        host = hostname;
    }
    else
    {
        writeLog("Common: getHostName: failed to get the hostname.", SCM::FAILED);
        return "unknown";
    }
#else
    host = "platform";
#endif
    return host;
}

int Common::convertToUtc(const string &sys_time, string &utc_time)
{
    if (!isValidSysLogTimeString(sys_time))
    {
        return SCM::FAILED;
    }
    std::stringstream ss(sys_time);
    string year, month, day, time;
    std::tm tm = {};
    int m = 0, d;
    month = trim(sys_time.substr(0, 4));
    for (; m < (int)SCM::MONTHS.size(); m++)
    {
        if (SCM::MONTHS[m] == month)
        {
            m++;
            break;
        }
    }
    month = "";
    if (m <= 9 && m > 0)
    {
        month = "0";
    }
    month += std::to_string(m);
    day = trim(sys_time.substr(4, 6));
    d = std::stoi(day);
    day = "";
    if (d <= 9 && d > 0)
    {
        day = "0";
    }
    day += std::to_string(d);
    time = trim(sys_time.substr(6, 15));
    std::time_t current_time = std::time(nullptr);
    std::tm *currentTm = std::localtime(&current_time);
    tm.tm_year = currentTm->tm_year;
    tm.tm_year += 1900;

    utc_time = std::to_string(tm.tm_year) + "-" + month + "-" + day + " " + time;
    return SCM::SUCCESS;
}

int Common::convertSysLogToUtcFormat(const string &sys_time, string &utc_time)
{
    // Assuming the syslog time format is like "Jul 21 00:43:35"
    std::tm tm = {};
    std::stringstream ss(sys_time);

    std::string month_str, day_str, time_str;
    ss >> month_str >> day_str >> time_str;

    // Convert month abbreviation to month number
    const char *const month_names[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    for (int i = 0; i < 12; ++i)
    {
        if (month_str == month_names[i])
        {
            tm.tm_mon = i;
            break;
        }
    }

    // Parse day and time
    tm.tm_mday = std::stoi(day_str);
    sscanf(time_str.c_str(), "%d:%d:%d", &tm.tm_hour, &tm.tm_min, &tm.tm_sec);

    // Get current year
    std::time_t current_time = std::time(nullptr);
    std::tm *current_tm = std::localtime(&current_time);
    tm.tm_year = current_tm->tm_year; // Current year
    tm.tm_year += 1900;

    // Convert to UTC time
    std::time_t utc_time_t = std::mktime(&tm);

    // Format UTC time in the desired format
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S") << ".9111923+05:30";

    utc_time = oss.str();
    return SCM::SUCCESS;
}

std::string Common::convertDpkgTimeToUtcFormat(const std::string &local_time_str)
{
    // Regular expression to match the local time format
    std::string utc_time_str = local_time_str;

    // Replace space with 'T'
    utc_time_str[10] = 'T';

    // Append ".3785938+00:00" to the end of the string
    utc_time_str += ".3785938+00:00";

    return utc_time_str;
}

void Common::writeLog(const string &log)
{
    std::lock_guard<std::mutex> lm(logfile_mutex);
    {
        if (!Common::logfp.is_open())
        {
            Common::logfp.open(SCM::Config::LOG_PATH, std::ios::app);
        }
        Common::logfp.write(log.c_str(), log.size());

        Common::backupLogFile(SCM::Config::LOG_PATH);
    }
    return;
}

void Common::writeLog(const string &log, int log_level)
{
#if NO_DEBUG && log_level == DEBUG
    return;
#endif

    string time = os::getCurrentTime();
    string line;
    switch (log_level)
    {
    case SCM::SUCCESS:
        line = time + " : [SUCCESS] " + log;
        break;
    case SCM::INFO:
        line = time + " : [INFO] " + log;
        break;
    case SCM::FAILED:
        line = time + " : [ERROR] " + log;
        break;
    case SCM::WARNING:
        line = time + " : [WARNING] " + log;
        break;
    case SCM::CRITICAL:
        line = time + " : [CRITICAL] " + log;
        break;
    case SCM::DEBUG:
        line = time + " : [DEBUG] " + log;
    }
    line += "\n";
    std::lock_guard<std::mutex> lm(logfile_mutex);
    {
        if (SCM::is_syslog_enabled)
        {
#if __linux__
            syslog(LOG_INFO, " %s", line.c_str());
#endif
        }
        else
        {
            if (!Common::logfp.is_open())
            {
                Common::logfp.open(SCM::Config::LOG_PATH, std::ios::app);
            }
            Common::logfp.write(line.c_str(), line.size());
            Common::logfp.flush();
        }
    }
    Common::backupLogFile(SCM::Config::LOG_PATH);
    return;
}

std::time_t Common::stringToTime(const string &datetime)
{
    const char *STANDARD_TIME_FORMAT = "%Y-%m-%d %H:%M:%S";
    std::tm tm = {};
    std::istringstream ss(datetime);
    ss >> std::get_time(&tm, STANDARD_TIME_FORMAT);
    return std::mktime(&tm);
}
std::time_t Common::utcStringToTime(const string &str)
{
    struct tm timeinfo;
    std::istringstream ss(str);

    // Extract date and time components
    ss >> timeinfo.tm_year;
    timeinfo.tm_year -= 1900; // Adjust for tm_year offset
    ss.ignore(1);             // Skip '-'
    ss >> timeinfo.tm_mon;
    timeinfo.tm_mon -= 1; // Adjust for tm_mon offset
    ss.ignore(1);         // Skip '-'
    ss >> timeinfo.tm_mday;
    ss.ignore(1); // Skip 'T'

    ss >> timeinfo.tm_hour;
    ss.ignore(1); // Skip ':'
    ss >> timeinfo.tm_min;
    ss.ignore(1); // Skip ':'
    ss >> timeinfo.tm_sec;

    // Set remaining timeinfo fields
    timeinfo.tm_isdst = 0; // Daylight Saving Time not specified
    timeinfo.tm_sec = 0;

    return mktime(&timeinfo);
}

void Common::printNextExecutionTime(std::tm next_time_info)
{
    char buffer[80];
    std::strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", &next_time_info);
    std::string next_time_str(buffer);
    DEBUG(" next execution time: " + next_time_str);
}

void Common::printDuration(const std::chrono::duration<double> &duration)
{
    int hours = std::chrono::duration_cast<std::chrono::hours>(duration).count();
    auto remaining_duration = duration - std::chrono::hours(hours);
    int minutes = std::chrono::duration_cast<std::chrono::minutes>(remaining_duration).count();
    remaining_duration -= std::chrono::minutes(minutes);
    int seconds = static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(remaining_duration).count());
    DEBUG("duration until next execution: " + std::to_string(hours) + " hours, " + std::to_string(minutes) + " minutes, " + std::to_string(seconds) + " seconds.");
}

void Common::pauseTillNextExecution(const cron::cronexpr &cron)
{
    std::chrono::system_clock::time_point current_time = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(current_time);
    std::time_t next = cron::cron_next(cron, time);
    std::chrono::system_clock::time_point target_point = std::chrono::system_clock::from_time_t(next);
    std::tm next_time_info;
    localtime_r(&next, &next_time_info); // Use localtime_r for Unix-based systems
    printNextExecutionTime(next_time_info);
    std::chrono::duration<double> duration = target_point - current_time;
    Common::printDuration(duration);
    std::this_thread::sleep_for(duration);
    return;
}

void Common::parseArguments(int argc, char *argv[])
{
    std::vector<std::string> args(argv, argv + argc);
    if (std::find(args.begin(), args.end(), "--debug") != args.end() || std::find(args.begin(), args.end(), "-d") != args.end())
    {
        debug_enabled = true;
    }
    if (std::find(args.begin(), args.end(), "--console") != args.end() || std::find(args.begin(), args.end(), "-c") != args.end())
    {
        console_log_enabled = true;
    }
}

vector<string> Common::toVector(const string &line, const char sep)
{
    vector<string> names;
    if (line.find(sep) == string::npos && line.empty())
    {
        DEBUG("Seperator not found in string: ", std::to_string(sep));
        names.push_back(Common::trim(line));
        return names;
    }
    std::stringstream iss(line);
    string name;
    while (std::getline(iss, name, sep))
    {
        names.push_back(Common::trim(name));
    }
    return names;
}

string Common::getErrorString(const int errorCode)
{
    switch (errorCode)
    {
    case SUCCESS:
        return "SUCCESS";
    case MQTT_REQUEST_EMPTY_OR_NULL_ACTION_TYPE:
        return "MQTT_REQUEST_EMPTY_OR_NULL_ACTION_TYPE";
    case MQTT_REQUEST_INVALID_RESPONSE_TYPE:
        return "MQTT_REQUEST_INVALID_RESPONSE_TYPE";
    case MQTT_REQUEST_EMPTY_OR_NULL_SOURCE_ID:
        return "MQTT_REQUEST_EMPTY_OR_NULL_SOURCE_ID";
    case MQTT_REQUEST_EMPTY_OR_NULL_TARGET_ID:
        return "MQTT_REQUEST_EMPTY_OR_NULL_TARGET_ID";
    case MQTT_REQUEST_EMPTY_OR_NULL_REQUEST_ID:
        return "MQTT_REQUEST_EMPTY_OR_NULL_REQUEST_ID";
    case MQTT_REQUEST_INVALID_LOG_TYPE:
        return "MQTT_REQUEST_INVALID_LOG_TYPE";
    case SCM::Tpm::TPM2_ABRMD_NOT_ACTIVE:
        return "TPM2_ABRMD_NOT_ACTIVE";
    case SCM::Tpm::TPM2_ABRMD_SIMULATOR_ACTIVE:
        return "TPM2_ABRMD_SIMULATOR_ACTIVE";
    case SCM::Tpm::TPM2_DEVICE_NOT_EXIST:
        return "TPM2_DEVICE_NOT_EXIST";
    case SCM::Tpm::TSS2_LIBRARY_NOT_EXIST:
        return "TSS2_LIBRARY_NOT_EXIST";
    default:
        return "Unknown Error Code";
    }
}
