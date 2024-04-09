#ifndef ANALYSIS_REPOSITORY
#define ANALYSIS_REPOSITORY
 
#pragma once
 
#include "util/config.hpp"
#include "ids/ids_entity.hpp"
 
class ids_repository
{
public:
    std::unordered_map<string, decoder> get_decoder_list()
    {
        return decoder_list;
    }
    std::unordered_map<string, std::unordered_map<int, aconfig>> get_aconfig_rules()
    {
        return rules;
    }
    bool read_config_files(const string& decoder_path, const string& rule_path)
    {
        int result = config_s.read_decoder_config(decoder_path, decoder_list);
        if (result == Audit::FAILED)
        {
            return false;
        }
        result = config_s.read_xml_rule_config(rule_path, rules);
        if (result == Audit::FAILED)
        {
            return false;
        }
        return true;
    }
    int save(const vector<log_event>& alerts)
    {
        if (alerts.size() == 0)
        {
            DEBUG("no decoder matched");
            //agent_utils::write_log("ids_repository: save: no decoder matched", INFO);
            return Audit::SUCCESS;
        }
        string filePath = os::create_json_file("log-analysis-report");
        Json::Value json;
        json["OrgId"] = 5268;
        json["Source"] = os::host_name;
        json["AppName"] = "system_events";
        json["Alerts"] = Json::Value(Json::arrayValue);
        Json::Value alert;
        Json::StreamWriterBuilder writer_builder;
 
        for (const auto& log : alerts)
        {
            aconfig config = get_rule(log.group, log.rule_id);
            if (config.id <= 0)
            {
                DEBUG("unrecognized rule, rule_id=" + std::to_string(log.rule_id) + " RuleGroup=" + log.group);
                //agent_utils::write_log("ids_repository: save: unrecognized rule, rule_id=" + std::to_string(log.rule_id) + " RuleGroup=" + log.group, WARNING);
            }
            else
            {
                // print_log_details(config, log);
                aconfig child = config;
                // string group;
                alert["TimeStamp"] = log.timestamp;
                alert["User"] = log.user;
                alert["Program"] = log.program;
                alert["Message"] = log.message;
                alert["Category"] = log.decoded;
                alert["MatchedRule"] = config.id;
                alert["LogLevel"] = config.level;
                while (child.if_sid > 0)
                {
                    child = get_rule(log.group, child.if_sid);
                }
 
                // group = (config.group.empty()) ? child.group : config.group;
 
                alert["Section"] = log.group;
                alert["Description"] = (config.description.empty()) ? child.description : config.description;
                json["Alerts"].append(alert);
            }
        }
        std::ofstream ofile(filePath); /*need update*/
        std::unique_ptr<Json::StreamWriter> writer(writer_builder.newStreamWriter());
        writer->write(json, &ofile);
        ofile.close();
        DEBUG("log written to" + filePath);
        //agent_utils::write_log("ids_repository: save: log written to " + filePath, SUCCESS);
        return Audit::SUCCESS;
    }
 
    aconfig get_rule(const string& group, const int rule_id)
    {
        aconfig config;
        for (const auto& parent : rules)
        {
            for (const auto& child : parent.second)
            {
                if (child.first == rule_id)
                {
                    config = child.second;
                    break;
                }
            }
        }
        return config;
    }
 
    int print_log_details(const aconfig& rule_info, const log_event& log_info)
    {
        aconfig child = rule_info;
        string group = log_info.group;
        std::cout << "Timestamp : " << log_info.timestamp << "\n";
        std::cout << "user      : " << log_info.user << "\n";
        std::cout << "program   : " << log_info.program << "\n";
        std::cout << "log       : " << log_info.message << "\n";
        std::cout << "category  : " << log_info.decoded << "\n";
        std::cout << "rule      : " << child.id << "\n";
        std::cout << "level     : " << child.level << "\n";
        while (child.if_sid > 0)
        {
            child = get_rule(log_info.group, child.if_sid);
        }
        if (rule_info.description.empty())
        {
            group = child.decoded_as.empty() ? log_info.group : child.decoded_as;
        }
        else
        {
            group = rule_info.decoded_as.empty() ? log_info.group : rule_info.decoded_as;
        }
        std::cout << "group  : " << group << "\n";
        if (rule_info.description.empty())
        {
            std::cout << "Description: " << child.description << "\n";
        }
        else
        {
            std::cout << "Description: " << rule_info.description << "\n";
        }
        return Audit::SUCCESS;
    }
private:
    config config_s;
    std::unordered_map<string, std::unordered_map<int, aconfig>> rules;
    std::unordered_map<string, decoder> decoder_list;
};
#endif
//ids_repository