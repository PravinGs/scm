#ifndef LOG_SERVICE_IMPL_HPP
#define LOG_SERVICE_IMPL_HPP
#pragma once
#include "LogService.hpp"

class LogServiceImpl : public LogService
{
public:
    int getSysLog(LogEntity &entity, vector<string> &logs)
    {
        int result = SCM::SUCCESS;
        if (entity.name == "syslog" || entity.name == "auth")
        {
            /*if (entity.remote == 'y' || entity.remote == 'Y')
            {
                UdpQueue queue;
                result = read_remote_syslog(queue, logs);
                queue.stop();
            }
            else
            {
                result = readSysLogFile(entity, logs);
            }*/
            result = readSysLogFile(entity, logs);
        }
        else if (entity.name == "dpkg")
        {
            result = readDpkgFile(entity, logs);
        }
        return result;
    }

    int getSysLog()
    {
        return SCM::SUCCESS;
    }

    int getAppLog() { return SCM::SUCCESS; }

private:
    bool parseLogCategory(string &line, const vector<string> &log_levels)
    {
        bool result = false;
        int max_level = 0;
        string network = "network";
        string ufw = "ufw";
        string lowercase_line = line;
        auto it = std::find(log_levels.begin(), log_levels.end(), "all");
        if (it != log_levels.end())
        {
            line += "|0";
            result = true;
        }
        else
        {
            for (string level : log_levels)
            {
                std::transform(lowercase_line.begin(), lowercase_line.end(), lowercase_line.begin(), [](unsigned char c)
                               { return std::tolower(c); });
                std::transform(level.begin(), level.end(), level.begin(), [](unsigned char c)
                               { return std::tolower(c); });
                if (lowercase_line.find(level) != std::string::npos)
                {
                    if (log_parser_level[Common::toLowerCase(level)] > max_level)
                    {
                        max_level = log_parser_level[Common::toLowerCase(level)];
                        result = true;
                    }
                }
                if (level == "none")
                {
                    max_level = log_parser_level[Common::toLowerCase(level)];
                    result = true;
                }
            }
            line += "|" + std::to_string(max_level);
        }

        if (lowercase_line.find(network) != std::string::npos)
        {
            line += "|" + network;
        }
        else if (lowercase_line.find(ufw) != std::string::npos)
        {
            line += "|" + ufw;
        }
        else
        {
            line += "|sys";
        }
        return result;
    }

    // TODO read compressed file if expected time is not avialable.
    int readSysLogFile(LogEntity &entity, vector<string> &logs)
    {
        string standard_time_format_string, line;
        fstream file(entity.read_path, std::ios::in);
        if (!file.is_open())
        {
            LOG_ERROR(SCM::FILE_ERROR + entity.read_path);
            return SCM::FAILED;
        }
        while (std::getline(file, line))
        {
            // Add it to temproary location
            if (line.empty())
            {
                continue;
            }
            Common::convertToUtc(line.substr(0, 15), standard_time_format_string);        /* This func convert to standard time format */
            std::time_t current_time = Common::stringToTime(standard_time_format_string); /* Convert string time to time_t format for comparision between time_t objects */
            if (current_time < entity.last_read_time)
            {
                continue;
            }
            // if (current_time > entity.end_time)
            // {
            //     break;
            // }
            string log, token;

            bool is_required = true;
            int index = 0;
            log += Common::convertDpkgTimeToUtcFormat(standard_time_format_string);
            std::stringstream stream(line.substr(16));
            while (std::getline(stream, token, entity.delimeter) && index < 3)
            {
                if (index == 2)
                {
                    string message = token;
                    while (std::getline(stream, token, entity.delimeter))
                    {
                        message += entity.delimeter + token;
                    }
                    log += "|" + message;
                    is_required = parseLogCategory(log, entity.log_levels);
                    index = 4;
                    continue;
                }
                log += "|" + token;
                index++;
            }
            if (is_required)
            {
                logs.push_back(log);
                entity.count += 1;
            }
            std::time_t current_reading_time = Common::stringToTime(entity.current_read_time);
            if (current_time > current_reading_time)
            {
                entity.current_read_time = standard_time_format_string;
            }
            if (current_time == entity.last_read_time)
            {
                entity.is_empty = false;
            }
        }
        file.close();

        // if (both start and endtime should be found )
        if (entity.is_empty && entity.read_path.find_last_of('1') != string::npos) // read path should not endswoth .1
        {
            entity.read_path += ".1";
            readSysLogFile(entity, logs);
        } // segmentation fault.
        // still entity is empty and read path endswoth .1
        //
        return SCM::SUCCESS;
    }

    int readDpkgFile(LogEntity &entity, vector<string> &logs)
    {
        string standard_time_format_string, line;
        fstream file(entity.read_path, std::ios::in);
        if (!file)
        {
            LOG_ERROR(SCM::FILE_ERROR + entity.read_path);
            return SCM::FAILED;
        }

        while (std::getline(file, line))
        {
            string log, temp;
            string current_time_string = line.substr(0, 19);
            std::time_t current_time = Common::stringToTime(current_time_string); /* Convert string time to time_t format for comparision between time_t objects */
            string utc_time = Common::convertDpkgTimeToUtcFormat(current_time_string);
            if (current_time < entity.last_read_time)
            {
                continue;
            }
            log += utc_time;
            log += "|" + os::host_name;
            log += "| dpkg";
            temp = line.substr(20);
            log += "|" + temp;
            entity.count += 1;
            logs.push_back(log);
            std::time_t current_reading_time = Common::stringToTime(entity.current_read_time);
            if (current_time > current_reading_time)
            {
                entity.current_read_time = current_time_string;
            }
            if (current_time == entity.last_read_time)
            {
                entity.is_empty = false;
            }
        }
        if (entity.is_empty)
        {
            entity.read_path += ".1";
            readDpkgFile(entity, logs);
        }
        return SCM::SUCCESS;
    }

private:
    map<string, int> log_parser_level{{"none", 0}, {"trace", 1}, {"debug", 2}, {"warning", 3}, {"error", 4}, {"critical", 5}}; /**< A private constant map<string, int> for system log name. */

public:
    int readSysLogFile(LogEntity &entity, vector<string> &logs, const std::time_t &end_time, int &counter)
    {

        if (end_time < entity.last_read_time)
        {
            LOG_ERROR("End Time should not greater than start time");
            return SCM::WARNING;
        }
        int result = SCM::SUCCESS;
        string line;
        fstream file(entity.read_path, std::ios::in);
        if (!file.is_open())
        {
            LOG_ERROR(SCM::FILE_ERROR + entity.read_path);
            return SCM::FAILED;
        }
        while (std::getline(file, line))
        {
            if (line.empty())
                continue;

            if (formatSyslog(entity, line, end_time, logs) == -1)
                break;
        }
        file.close();

        // if (both start and endtime should be found )
        if (!entity.start_filter || !entity.end_filter || entity.is_empty) // read path should not endswoth .1
        {
            counter++;
            if (counter > 1)
            {
                string readPath = entity.read_path;
                size_t lastDigitPos = readPath.find_last_of("0123456789");

                // Check if a digit was found and the extension follows it
                if (lastDigitPos != string::npos ) //&& lastDigitPos + 1 == readPath.find(".gz")
                {
                    // Extract the path up to (but not including) the last digit
                    string archiveFilePath = readPath.substr(0, lastDigitPos);
                    // Append the counter and ".gz" extension
                    archiveFilePath += std::to_string(counter) + ".gz";
                    entity.read_path = archiveFilePath;
                    result = readArchiveSyslogFile(entity, logs, end_time, counter);
                }
            }
            else
            {
                entity.read_path += ".1";
                result = readSysLogFile(entity, logs, end_time, counter);
            }
        }
        return result;
    }

    int formatSyslog(LogEntity &entity,string &line, const std::time_t &end_time, vector<string> &logs)
    {
        string standard_time_format_string;
        Common::convertToUtc(line.substr(0, 15), standard_time_format_string);            /* This func convert to standard time format */
        std::time_t current_log_time = Common::stringToTime(standard_time_format_string); /* Convert string time to time_t format for comparision between time_t objects */
        if (current_log_time < entity.last_read_time)
        {
            return 0;
        }
        
        if (end_time != 1 && current_log_time >= end_time)
        {
            entity.end_filter = true;
            DEBUG("End time [" + standard_time_format_string + "] reached no further reading in this file" + entity.read_path);
            return -1;
        }
        string log, token;
        bool is_required = true;
        int index = 0;
        log += Common::convertDpkgTimeToUtcFormat(standard_time_format_string);
        std::stringstream stream(line.substr(16));
        while (std::getline(stream, token, entity.delimeter) && index < 3)
        {
            if (index == 2)
            {
                string message = token;
                while (std::getline(stream, token, entity.delimeter))
                {
                    message += entity.delimeter + token;
                }
                log += "|" + message;
                is_required = parseLogCategory(log, entity.log_levels);
                index = 4;
                continue;
            }
            log += "|" + token;
            index++;
        }
        if (is_required)
        {
            logs.push_back(log);
            entity.count++;
        }
        std::time_t current_reading_time = Common::stringToTime(entity.current_read_time);
        if (current_log_time > current_reading_time)
        {
            entity.current_read_time = standard_time_format_string;
            entity.start_filter = true;
        }
        entity.is_empty = false;

        return 0;
    }

    int readArchiveSyslogFile(LogEntity &entity, vector<string> &logs, const std::time_t &end_time, int &counter)
    {
        gzFile file = gzopen(entity.read_path.c_str(), "rb");
        if (!file)
        {
            std::cerr << "Cannot open gzip file: " << entity.read_path << std::endl;
            return SCM::FAILED;
        }
        char buffer[1024];
        while (gzgets(file, buffer, sizeof(buffer)) != Z_NULL )
        {
            string line(buffer);
            formatSyslog(entity, line, end_time, logs);
        }
        if (gzeof(file))
        {
            gzclose(file);
        } else {
            int err;
            const char* errorString = gzerror(file, &err);
            if (err != Z_ERRNO)
            {
                std::cout << "Error reading gzip file: " << errorString << '\n';
            } else{
                perror("Error reading gzip file");
            }
            gzclose(file);
            return SCM::FAILED;
        }

        return SCM::SUCCESS;
    }
};
#endif // !LOG_SERVICE_IMPL_HPP

// int main() {
//     std::string zipFilePath = "your_log_file.zip";
//     std::string fileNameInZip = "your_log_file.log";

//     if (!extract_and_read_zip(zipFilePath, fileNameInZip)) {
//         std::cerr << "Failed to extract and read the log file from the zip archive." << std::endl;
//         return 1;
//     }

//     return 0;
// }
