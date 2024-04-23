#ifndef ROOT_CHECK_HPP
#define ROOT_CHECK_HPP
#pragma once

#include "dev_check.hpp"
#include "interface_check.hpp"
#include "ports_check.hpp"
#include "process_check.hpp"
#include "trojen_check.hpp"
#include "sysfile_check.hpp"
#include "command_service.hpp"

class root_check
{
public:
    root_check()
    {
        root_check(Audit::IDS::TROJAN_SOURCE_FILE, Audit::IDS::SYS_SOURCE_FILE);
    }

    root_check(const string &trojan_source_file, const string &sys_source_file): c_service(std::make_unique<command_service_impl>()),trojan_source_file(trojan_source_file), sys_source_file(sys_source_file)
    {
    }

    string start()
    {
        Json::Value root_check;
        DEBUG("starting trojan root check.");
        vector<string> reports;
        Json::StreamWriterBuilder writer_builder;
        if (t_check.check(trojan_source_file, reports))
        {
            root_check["Trojan_check"] = Json::Value(Json::arrayValue);
            for (string report : reports)
            {
                Json::Value json;
                size_t mid = report.find_first_of(',');
                json["location"] = report.substr(0, mid);
                json["pattern"] = report.substr(mid + 1);
                root_check["Trojan_check"].append(json);
            }
            reports.clear();
            DEBUG("trojan root check completed successfully.");
        }
        else
        {
            LOG_ERROR("trojan root check failed");
        }
        DEBUG("completed trojan root check.");
        DEBUG("starting sysfiles root check.");
        if (s_check.check(sys_source_file, reports))
        {
            root_check["SysFileCheck"] = Json::Value(Json::arrayValue);
            for (string report : reports)
            {
                Json::Value json;
                size_t mid = report.find_first_of(',');
                json["name"] = report.substr(mid + 1);
                json["location"] = report.substr(0, mid);
                root_check["SysFileCheck"].append(json);
            }
            reports.clear();
            DEBUG("sysfiles root check completed successfully.");
        }
        else
        {
            LOG_ERROR("sysfiles root check failed");
        }
        DEBUG("completed sysfiles root check.");
        DEBUG("starting network interface root check.");
        if (if_check.check(reports))
        {
            root_check["PromiscuousCheck"] = Json::Value(Json::arrayValue);
            for (string report : reports)
            {
                Json::Value json;
                json["interface"] = report;
                root_check["PromiscuousCheck"].append(json);
            }
            reports.clear();
            DEBUG("network interface  root check completed successfully.");
        }
        else
        {
            LOG_ERROR("network interface root check failed");
        }
        DEBUG("completed network interface root Check.");
        DEBUG("starting dev root check.");
        if (d_check.check(reports))
        {
            root_check["HiddenFileCheck"] = Json::Value(Json::arrayValue);
            for (string report : reports)
            {
                Json::Value json;
                json["path"] = report;
                root_check["HiddenFileCheck"].append(json);
            }
            reports.clear();
            DEBUG("dev root check completed successfully.");
        }
        else
        {
            LOG_ERROR("dev root check failed");
        }
        DEBUG("completed dev root check.");
        execute_commands(root_check);
        // std::fstream file("/home/champ/SecurityComponent/sys_checkreport.json", std::ios::out);
        // std::unique_ptr<Json::StreamWriter> writer(writer_builder.newStreamWriter());
        // writer->write(root_check, &file);
        // file.close();
        DEBUG("completed process root check");
        return root_check.toStyledString();
    }

    ~root_check()
    {
      
    }

private:
    void execute_commands(Json::Value &json)
    {
        for (const string &s : report_commands)
        {
            vector<string> output;
            size_t mid = s.find_first_of(':');
            c_service->read_command(s.substr(mid + 1), output);
            string name = s.substr(0, mid);
            json[name] = Json::Value(Json::arrayValue);
            for (string line : output)
            {
                json[name].append(line);
            }
        }
    }

private:
    std::unique_ptr<command_service> c_service;
    trojan_check t_check;
    sys_check s_check;
    process_check p_check;
    port_check pt_check;
    dev_check d_check;
    interface_check if_check;
    string trojan_source_file;
    string sys_source_file;
    vector<string> report_commands = {
        "ApparmorStatus:aa-status",
        "PortDetails:netstat -tuln",
        "NetworkConnection:lsof -i -P",
        "LoginActivity:last -n 5",
    };
};

#endif