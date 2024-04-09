#include "common.hpp"

fstream common::logfp;
std::mutex logfile_mutex;
bool common::debug = false;

int common::setup_logger(const string& log_file)
{
    int result = Audit::SUCCESS;
#if __linux__
    if (os::is_file_exist(Audit::Config::SYSLOG_FILE)){
        Audit::is_syslog_enabled = true;
        return Audit::SUCCESS;
    }
    /*std::uintmax_t s1 = get_file_size(Audit::Config::SYSLOG_FILE);
    if (s1 > 0L)
    {
        Audit::is_syslog_enabled = true;
        return Audit::SUCCESS;
    }
    else if (s1 == 0L)
    {
        syslog(LOG_INFO, "Syslog probe message for testing");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::uintmax_t s2 = get_file_size(Audit::Config::SYSLOG_FILE);
        if (s2 > s1)
        {
            Audit::is_syslog_enabled = true;
        }
        else
        {
        }
    }*/
#endif
    if (!Audit::is_syslog_enabled && !os::is_file_exist(log_file))
    {
        result = os::create_file(log_file);
    }
    return result;
}

int common::backup_log_file(const string& file_path)
{
    if (os::is_file_exist(file_path) && is_file_greater_than_max_size(file_path) && os::compress_file(file_path) == Audit::SUCCESS)
    {
        common::logfp.close();
        common::logfp.open(file_path, std::ios::out);
        return Audit::SUCCESS;
    }
    else 
    {
        return Audit::FAILED;
    }
}

bool common::is_file_greater_than_max_size(const string &log_file)
{
    std::uintmax_t file_size = get_file_size(Audit::Config::LOG_PATH);
    double max_file_size = static_cast<double>(file_size) / (1024 * 1024);
    return max_file_size < 5.0;
}

string common::to_lower_case(string &str)
{
    string lower_case_string;
    for (char ch : str)
    {
        lower_case_string += std::tolower(ch);
    }
    return lower_case_string;
}

bool common::is_valid_syslog_time_string(const string &sys_time)
{
    std::regex pattern(R"([A-Za-z]{3} \d{1,2} \d{2}:\d{2}:\d{2})");
    return std::regex_match(sys_time, pattern);
}

string common::create_log_cache_file(const string &app_name)
{
    string file_path = Audit::Config::BASE_CONFIG_DIR;
    if (!os::is_dir_exist(file_path) && os::create_dir(file_path) != Audit::SUCCESS)
    {
        return "";
    }
    file_path += Audit::Config::BASE_CONFIG_TMP;
    if (!os::is_dir_exist(file_path) && os::create_dir(file_path) != Audit::SUCCESS)
    {
        return "";
    }
    file_path += app_name;
    if (!os::is_file_exist(file_path) && os::create_file(file_path) != Audit::SUCCESS)
    {
        return "";
    }
    return file_path;
}

int common::update_log_written_time(const string &app_name, const string &time)
{
    string file_path = create_log_cache_file(app_name);
    fstream file(file_path, std::ios::out);
    if (!file.is_open())
    {
        LOG_ERROR(Audit::INVALID_FILE, file_path);
        // write_log("common: update_log_written_time: " + Audit::INVALID_FILE + file_path, FAILED);
        return Audit::FAILED;
    }
    file << time;
    LOG("time ", time, " updated to ", file_path);
    // write_log("common: update_log_written_time: time " + time + " updated to " + file_path, SUCCESS);
    file.close();
    return Audit::SUCCESS;
}

string common::trim(string line)
{
    const auto str_begin = line.find_first_not_of(" \t");
    if (str_begin == string::npos)
        return "";

    const auto strEnd = line.find_last_not_of(" \t");
    const auto strRange = strEnd - str_begin + 1;

    string str = line.substr(str_begin, strRange);
    return (str.length() >= 2 && str[0] == '"' && str[str.length() - 1] == '"') ? str.substr(1, str.length() - 2) : str;
}

string common::get_hostname()
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
        write_log("common: get_hostname: failed to get the hostname.", Audit::FAILED);
        return "unknown";
    }
#else
    host = "platform";
#endif
    return host;
}

int common::convert_syslog_to_utc_format(const string &sys_time, string &utc_time)
{
    if (!is_valid_syslog_time_string(sys_time))
    {
        return Audit::FAILED;
    }
    std::stringstream ss(sys_time);
    string year, month, day, time;
    std::tm tm = {};
    int m = 0, d;
    month = trim(sys_time.substr(0, 4));
    for (; m < (int)Audit::MONTHS.size(); m++)
    {
        if (Audit::MONTHS[m] == month)
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
    std::time_t current_time;
    std::time(&current_time);
    std::tm currentTm;
    localtime_r(&current_time, &currentTm);
    tm.tm_year = currentTm.tm_year;
    tm.tm_year += 1900;

    utc_time = std::to_string(tm.tm_year) + "-" + month + "-" + day + " " + time;
    return Audit::SUCCESS;
}

void common::write_log(const string &log)
{
    string time = os::get_current_time();
    string line = time + " " + log + "\n";
    std::lock_guard<std::mutex> lm(logfile_mutex);
    if (common::logfp.is_open())
    {
        common::logfp.write(line.c_str(), line.size());
    }
    common::backup_log_file(Audit::Config::LOG_PATH);
    return;
}

void common::write_log(const string &log, int log_level)
{
#if NO_DEBUG && log_level == DEBUG
    return;
#endif

    string time = os::get_current_time();
    string line;
    switch (log_level)
    {
    case Audit::SUCCESS:
        line = time + " : [SUCCESS] " + log;
        break;
    case Audit::INFO:
        line = time + " : [INFO] " + log;
        break;
    case Audit::FAILED:
        line = time + " : [ERROR] " + log;
        break;
    case Audit::WARNING:
        line = time + " : [WARNING] " + log;
        break;
    case Audit::CRITICAL:
        line = time + " : [CRITICAL] " + log;
        break;
    case Audit::DEBUG:
        line = time + " : [DEBUG] " + log;
    }
    line += "\n";
    std::lock_guard<std::mutex> lm(logfile_mutex);
    {
        if (Audit::is_syslog_enabled)
        {
#if __linux__
            syslog(LOG_INFO, " %s", line.c_str());
#endif
        }
        else
        {
            if (!common::logfp.is_open())
            {
                common::logfp.open(Audit::Config::LOG_PATH, std::ios::app);
            }
            common::logfp.write(line.c_str(), line.size());
            common::logfp.flush();
        }
    }
    common::backup_log_file(Audit::Config::LOG_PATH);
    return;
}

std::time_t common::string_to_time_t(const string &datetime)
{
    const char *STANDARD_TIME_FORMAT = "%Y-%m-%d %H:%M:%S";
    std::tm tm = {};
    std::istringstream ss(datetime);
    ss >> std::get_time(&tm, STANDARD_TIME_FORMAT);
    return std::mktime(&tm);
}

void common::print_next_execution_time(std::tm next_time_info)
{
    char buffer[80];
    std::strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", &next_time_info);
    std::string next_time_str(buffer);
    DEBUG(" next execution time: " + next_time_str);
    // common::write_log("common: print_next_execution_time: next execution time: " + next_time_str, DEBUG);
}

void common::print_duration(const std::chrono::duration<double> &duration)
{
    int hours = std::chrono::duration_cast<std::chrono::hours>(duration).count();
    auto remaining_duration = duration - std::chrono::hours(hours);
    int minutes = std::chrono::duration_cast<std::chrono::minutes>(remaining_duration).count();
    remaining_duration -= std::chrono::minutes(minutes);
    int seconds = static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(remaining_duration).count());
    DEBUG("duration until next execution: " + std::to_string(hours) + " hours, " + std::to_string(minutes) + " minutes, " + std::to_string(seconds) + " seconds.");
    // common::write_log("common: print_duration: duration until next execution: " + std::to_string(hours) + " hours, " + std::to_string(minutes) + " minutes, " + std::to_string(seconds) + " seconds.", DEBUG);
}

std::uintmax_t common::get_file_size(const string &file_name)
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

void common::pause_till_next_execution(const cron::cronexpr &cron)
{
    std::chrono::system_clock::time_point current_time = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(current_time);
    std::time_t next = cron::cron_next(cron, time);
    std::chrono::system_clock::time_point target_point = std::chrono::system_clock::from_time_t(next);
    std::tm next_time_info;
    localtime_r(&next, &next_time_info); // Use localtime_r for Unix-based systems
    print_next_execution_time(next_time_info);
    std::chrono::duration<double> duration = target_point - current_time;
    common::print_duration(duration);
    std::this_thread::sleep_for(duration);
    return;
}
// common.cpp