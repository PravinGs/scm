#ifndef SysCheck_SERVICE_HPP
#define SysCheck_SERVICE_HPP
#pragma once
 
 
#include "util/Common.hpp"

using namespace SCM;
class SysCheck
{
    public:
        int check(const string source_file, vector<string> & reports)
        {
            total = 0;
            errors = 0;
            rk_sys_count = 0;
            fstream file(source_file, std::ios::in);
 
            if (!file)
            {
                LOG_ERROR(source_file + " not exists");
                return SCM::FAILED;
            }
            string line;
            while (std::getline(file, line))
            {
                total++;
                /* Check rk_sys_count for IOB exception */
                if (rk_sys_count >= MAX_RK_SYS)
                {
                    LOG_ERROR("rk_sys count reached");
                    break;
                }
                /* continue comments and empty lines */
                if (line[0] == '#' || line.empty())
                {
                    continue;
                }
 
                int mid = (int)line.find_first_of('!');
                string file = Common::trim(line.substr(0, mid));
                int end = (int)line.find_first_of(':');
                string name = Common::trim(line.substr(mid + 1, end - mid - 1));
                string filePath = base_dir + file;
                if (file[0] == '*')
                {
                    string temp = file.substr(file.find_first_of('/'));
                    rk_sys_file[rk_sys_count] = temp;
                    rk_sys_name[rk_sys_count] = name;
                    if (rk_sys_name[rk_sys_count].empty() || rk_sys_file[rk_sys_count].empty()) 
                    {
                        DEBUG("could not acquire memory");
                        rk_sys_file[rk_sys_count].clear();
                        rk_sys_name[rk_sys_count].clear();
                    }
                    rk_sys_count++;
                    continue;
                }
 
                if (std::filesystem::exists(filePath))
                {
                    reports.push_back(name+","+file);
                    LOG_ERROR("rootkit " + name + " detected by the presence of file " + file);
                    errors++;
                }
            }
 
            file.close();
 
            DEBUG("total " + std::to_string(total) + " number of rootkit files processed.");
 
            if (errors > 0)
            {
                LOG("total " + std::to_string(errors) + " number of rootkit detected.");
            }
            return SUCCESS;
        }
private:
    string rk_sys_file[SCM::MAX_RK_SYS + 1];
    string rk_sys_name[SCM::MAX_RK_SYS + 1];
    int rk_sys_count = 0;
    const string base_dir = "/";
    int errors = 0;
    int total = 0;
 
};
 
#endif //SysCheck_SERVICE_HPP