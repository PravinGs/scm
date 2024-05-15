#ifndef RootCheck_SERVICE_HPP
#define RootCheck_SERVICE_HPP
#pragma once

#include "DevCheckService.hpp"
#include "NWInterfaceCheckService.hpp"
#include "PortsCheckService.hpp"
#include "ProcessCheckService.hpp"
#include "TrojenCheckService.hpp"
#include "SysCheckService.hpp"
#include "CommandService.hpp"

class RootCheck
{
public:
    RootCheck(): c_service(std::make_unique<CommandServiceImpl>()),trojan_source_file(SCM::IDS::TROJAN_SOURCE_FILE), sys_source_file(SCM::IDS::SYS_SOURCE_FILE)
    {
    }

    RootCheck(const string &trojan_source_file, const string &sys_source_file): c_service(std::make_unique<CommandServiceImpl>()),trojan_source_file(trojan_source_file), sys_source_file(sys_source_file)
    {
    }

    Json::Value start()
    {
        Json::Value root_check_report;
        DEBUG("starting trojan root check.");
        vector<string> reports;
        Json::StreamWriterBuilder writer_builder;
        if (t_check.check(trojan_source_file, reports))
        {
            root_check_report["TrojenCheck"] = Json::Value(Json::arrayValue);
            for (string report : reports)
            {
                Json::Value json;
                size_t mid = report.find_first_of(',');
                json["location"] = report.substr(0, mid);
                json["pattern"] = report.substr(mid + 1);
                root_check_report["TrojenCheck"].append(json);
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
            root_check_report["SysFileCheck"] = Json::Value(Json::arrayValue);
            for (string report : reports)
            {
                Json::Value json;
                size_t mid = report.find_first_of(',');
                json["name"] = report.substr(mid + 1);
                json["location"] = report.substr(0, mid);
                root_check_report["SysFileCheck"].append(json);
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
            root_check_report["PromiscuousCheck"] = Json::Value(Json::arrayValue);
            for (string report : reports)
            {
                Json::Value json;
                json["interface"] = report;
                root_check_report["PromiscuousCheck"].append(json);
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
            root_check_report["HiddenFileCheck"] = Json::Value(Json::arrayValue);
            for (string report : reports)
            {
                Json::Value json;
                json["path"] = report;
                root_check_report["HiddenFileCheck"].append(json);
            }
            reports.clear();
            DEBUG("dev root check completed successfully.");
        }
        else
        {
            LOG_ERROR("dev root check failed");
        }
        DEBUG("completed dev root check.");
        executeCommands(root_check_report);
        DEBUG("completed process root check");
        return root_check_report;
    }

    ~RootCheck()
    {
      
    }

private:
    void executeCommands(Json::Value &json)
    {
        for (const string &s : report_commands)
        {
            vector<string> output;
            size_t mid = s.find_first_of(':');
            c_service->readCommand(s.substr(mid + 1), output);
            string name = s.substr(0, mid);
            json[name] = Json::Value(Json::arrayValue);
            for (string line : output)
            {
                json[name].append(line);
            }
        }
    }

private:
    std::unique_ptr<CommandService> c_service;
    TrojenCheck t_check;
    SysCheck s_check;
    ProcessCheck p_check;
    PortCheck pt_check;
    DeviceCheck d_check;
    NwInterfaceCheckService if_check;
    string trojan_source_file;
    string sys_source_file;
    vector<string> report_commands = {
        "ApparmorStatus:aa-status",
        "PortDetails:netstat -tuln",
        "NetworkConnection:lsof -i -P",
        "LoginActivity:last -n 5",
    };
};

#endif //RootCheck_SERVICE_HPP