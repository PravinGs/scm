#ifndef DEV_FOLDER_CHECK
#define DEV_FOLDER_CHECK
#pragma once
/*
    #ifdef SOLARIS
        ".devfsadm_dev.lock",
        ".devlink_db_lock",
        ".devlink_db",
        ".devfsadm_daemon.lock",
        ".devfsadm_deamon.lock",
        ".devfsadm_synch_door",
        ".zone_reg_door",
#endif
*/
#include "util/common.hpp"
 
class dev_check
{
 
public:
    int read_dev_file(const string file_name, vector<string> & reports)
    {
        int result = Audit::SUCCESS;
        std::filesystem::path file_path(file_name);
        if (!std::filesystem::exists(file_path)) 
        {
            LOG_ERROR(Audit::INVALID_PATH + error);
            //agent_utils::write_log("dev_check: read_dev_file: " + INVALID_PATH + file_name, FAILED);
            return Audit::FAILED;
        }
        if (std::filesystem::is_directory(file_path)) 
        {
            return read_dev_dir(file_name, reports);
        }
        else if (std::filesystem::is_regular_file(file_path)) 
        {
            result = Audit::CRITICAL;
            string error = file_name;
            std::string op_msg = "File '" + error + "' present on /dev. Possible hidden file.";
            LOG_ERROR(op_msg);
            //agent_utils::write_log("dev_check: read_dev_file: " + op_msg, CRITICAL);
            dev_errors++;
        }
        return result;
    }
 
    int read_dev_dir(const string dir_name, vector<string> & reports)
    {
        if (dir_name.empty() || dir_name.length() > PATH_MAX) 
        {
            string error = dir_name;
            LOG_ERROR(Audit::INVALID_PATH + error);
            //agent_utils::write_log("dev_check: read_dev_dir: " + INVALID_PATH + error, FAILED);
            return Audit::FAILED;
        }
 
        for (const auto& entry : std::filesystem::directory_iterator(dir_name)) 
        {
            const auto& entry_name = entry.path().filename().string();
 
            // Ignore . and ..
            if (entry_name == "." || entry_name == "..") {
                continue;
            }
 
            dev_total++;
 
            // Check against the list of ignored files
            if (std::find(std::begin(IGNORE_DEV), std::end(IGNORE_DEV), entry_name) != std::end(IGNORE_DEV)) {
                continue;
            }
 
            // Check against the list of ignored full paths
            if (std::find(std::begin(IGNORE_DEV_FULL_PATH), std::end(IGNORE_DEV_FULL_PATH), entry.path().string()) != std::end(IGNORE_DEV_FULL_PATH)) {
                continue;
            }
 
            // Found a non-ignored entry in the directory, so process it
            int response = read_dev_file(entry.path().string(), reports);
            if (response == Audit::CRITICAL){
                reports.push_back(entry.path().string());
            }
        }
        return (0);
    }
 
    int check(vector<string> & reports)
    {
        const char *basedir = "/";
        char file_path[OS_SIZE_1024 + 1];
 
        dev_total = 0;
        dev_errors = 0;
        LOG("starting check_rc_dev");
        //agent_utils::write_log("dev_check: check: starting on check_rc_dev", INFO);
 
        snprintf(file_path, 1024, "%s/dev", basedir);
 
        read_dev_dir(file_path, reports);
        if (dev_errors == 0)
        {
            string output_message = "No problem detected on the /dev directory. Analyzed " + std::to_string(dev_total) + " files";
            DEBUG(output_message);
            //agent_utils::write_log("dev_check: check: " + output_message, SUCCESS);
        }
        return Audit::SUCCESS;
    }
private:
    int dev_errors;
    int dev_total;
    const vector<string> IGNORE_DEV = {
        "MAKEDEV", "README.MAKEDEV",
        "MAKEDEV.README", ".udevdb",
        ".udev.tdb", ".initramfs-tools",
        "MAKEDEV.local", ".udev", ".initramfs",
        "oprofile", "fd", "cgroup",
        ""
    };
    const vector<string> IGNORE_DEV_FULL_PATH = {
        "/dev/shm/sysconfig",
        "/dev/bus/usb/.usbfs",
        "/dev/shm",
        "/dev/gpmctl",
        ""
    };
}