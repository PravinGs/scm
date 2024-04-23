#ifndef SYSFILES_CHECK
#define SYSFILES_CHECK
#pragma once
 
 
#include "util/common.hpp"

using namespace Audit;
class sys_check
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
                return Audit::FAILED;
            }
            string line;
            while (std::getline(file, line))
            {
                total++;
                /* Check rk_sys_count for IOB exception */
                if (rk_sys_count >= MAX_RK_SYS)
                {
                    LOG_ERROR("rk_sys count reached");
                    //common::write_log("sys_check: check: rk_sys count reached", WARNING);
                    break;
                }
                /* continue comments and empty lines */
                if (line[0] == '#' || line.empty())
                {
                    continue;
                }
 
                int mid = (int)line.find_first_of('!');
                string file = common::trim(line.substr(0, mid));
                int end = (int)line.find_first_of(':');
                string name = common::trim(line.substr(mid + 1, end - mid - 1));
                string filePath = base_dir + file;
                if (file[0] == '*')
                {
                    string temp = file.substr(file.find_first_of('/'));
                    rk_sys_file[rk_sys_count] = temp;
                    rk_sys_name[rk_sys_count] = name;
                    if (rk_sys_name[rk_sys_count].empty() || rk_sys_file[rk_sys_count].empty()) 
                    {
                        DEBUG("could not acquire memory");
                        //common::write_log("sys_check: check: could not acquire memory", WARNING);
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
                    //common::write_log("sys_check: check: rootkit " + name + " detected by the presence of file " + file, CRITICAL);
                    errors++;
                }
            }
 
            file.close();
 
            DEBUG("total " + std::to_string(total) + " number of rootkit files processed.");
            //common::write_log("sys_check: check: total " + std::to_string(total) + " number of rootkit files processed.");
 
            if (errors > 0)
            {
                LOG("total " + std::to_string(errors) + " number of rootkit detected.");
                //common::write_log("sys_check: check: total " + std::to_string(errors) + " number of rootkit detected.");
            }
            return SUCCESS;
        }
private:
    string rk_sys_file[Audit::MAX_RK_SYS + 1];
    string rk_sys_name[Audit::MAX_RK_SYS + 1];
    int rk_sys_count = 0;
    const string base_dir = "/";
    int errors = 0;
    int total = 0;
 
};
 
#endif
//sysfilecheck.hpp