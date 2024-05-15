#ifndef TROJEN_CHECK_SERVICE_HPP
#define TROJEN_CHECK_SERVICE_HPP
#pragma once
 
#include "util/Common.hpp"
using namespace SCM;
class TrojenCheck
{
public:
    int check(const string& file_path, vector<string> & reports)
    {
        fstream fp(file_path, std::ios::in);
        string line;
        if (!fp)
        {
            LOG_ERROR(FILE_ERROR + file_path);
            return FAILED;
        }
        detected = 0;
        total = 0;
 
        while (std::getline(fp, line))
        {
            if (line.empty() || line[0] == '#')
                continue;
            int start = 0;
            int mid = (int)line.find_first_of('!');
            int end = (int)line.find_last_of('!');
            string search_binary = Common::trim(line.substr(start, mid));
            string pattern = Common::trim(line.substr(mid + 1, (end - mid - 1)));
            total++;
            for (string folder : root_folders)
            {
                string path = base_directory + folder + "/" + search_binary;
                if (!std::filesystem::exists(path))
                    continue;
                if (pattern_matching(path, pattern))
                {
                    detected = 1;
                    reports.push_back(path+","+pattern);
                    string error_message = "Trojaned version of file " + path + " detected. Signature used: " + pattern;
                    LOG_ERROR(error_message);
                }
            }
        }
 
        if (detected == 0)
        {
            string error_message = "No binaries with any trojan detected. Analyzed " + std::to_string(total) + " files.";
            LOG_ERROR(error_message);
            DEBUG(error_message);
        }
        return SUCCESS;
    }
private:
    bool pattern_matching(const string& file, const string& pattern)
    {
        std::ifstream fp(file, std::ios::binary);
 
        if (!fp.is_open())
        {
            LOG_ERROR(FILE_ERROR + file);
            return false;
        }
        char buffer[OS_SIZE_1024];
        string content;
        while (fp.read(buffer, OS_SIZE_1024))
        {
            content.append(buffer, fp.gcount());
        }
        if (fp.bad())
        {
            LOG_ERROR(FREAD_FAILED + file);
            return false;
        }
        std::regex regex(pattern);
        return std::regex_search(content, regex);
    }
private:
    vector<string> root_folders = { "bin", "sbin", "usr/bin", "usr/sbin" };
    const string base_directory = "/";
    int detected = 0;
    int total = 0;
};
 
#endif //TROJEN_CHECK_SERVICE_HPP