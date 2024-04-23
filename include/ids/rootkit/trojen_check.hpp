#ifndef TROJEN_CHECK_CPP
#define TROJEN_CHECK_CPP
#pragma once
 
#include "util/common.hpp"
using namespace Audit;
class trojan_check
{
public:
    int check(const string& filePath, vector<string> & reports)
    {
        fstream fp(filePath, std::ios::in);
        string line;
        if (!fp)
        {
            LOG_ERROR(FILE_ERROR + filePath);
            //DEBUG( + FILE_ERROR + filePath, FAILED);
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
            string searchBinary = common::trim(line.substr(start, mid));
            string pattern = common::trim(line.substr(mid + 1, (end - mid - 1)));
            total++;
            for (string folder : rootFolders)
            {
                string path = baseDirectory + folder + "/" + searchBinary;
                if (!std::filesystem::exists(path))
                    continue;
                if (pattern_matching(path, pattern))
                {
                    detected = 1;
                    reports.push_back(path+","+pattern);
                    string errorMessage = "Trojaned version of file " + path + " detected. Signature used: " + pattern;
                    LOG_ERROR(errorMessage);
                    //DEBUG( + errorMessage, CRITICAL);
                }
            }
        }
 
        if (detected == 0)
        {
            string errorMessage = "No binaries with any trojan detected. Analyzed " + std::to_string(total) + " files.";
            LOG_ERROR(errorMessage);
            DEBUG(errorMessage);
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
            //DEBUG("trojan_check: pattern_matching: " + FILE_ERROR + file, FAILED);
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
            //DEBUG("trojen_check: pattern_matching: " + FREAD_FAILED + file, FAILED);
            return false;
        }
        std::regex regex(pattern);
        return std::regex_search(content, regex);
    }
private:
    vector<string> rootFolders = { "bin", "sbin", "usr/bin", "usr/sbin" };
    const string baseDirectory = "/";
    int detected = 0;
    int total = 0;
};
 
#endif
//trojencheck.hpp