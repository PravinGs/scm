#ifndef ANALYSIS_REPosITORY
#define ANALYSIS_REPosITORY

#pragma once

#include "util/ConfigService.hpp"
#include "ids/IdsEntity.hpp"

class IdsRepository
{
public:
    std::unordered_map<string, decoder> getDecoderList()
    {
        return decoder_list;
    }
    
    std::unordered_map<string, std::unordered_map<int, AConfigService>> getAllAConfigServiceRules()
    {
        return rules;
    }
    
    bool readConfigServiceFiles(const string &decoder_path, const string &rule_path)
    {
        int result = configService.readDecoderConfigFile(decoder_path, decoder_list);
        if (result == SCM::FAILED)
        {
            return false;
        }
        result = configService.readXmlRuleConfigFile(rule_path, rules);
        if (result == SCM::FAILED)
        {
            return false;
        }
        return true;
    }

    int saveRootAnalysisReport(const Json::Value &json_string)
    {
        string file_path = os::createJSONFile("root-analysis");
        Json::Value json;
        json["OrgId"] = 5268;
        json["Source"] = os::host_name;
        json["AppName"] = "Rootkit Analysis and Linux Network commands";
        json["Alerts"] = Json::Value(Json::arrayValue);
        std::ofstream ofile(file_path); /*need update*/
        Json::StreamWriterBuilder writer_builder;
        std::unique_ptr<Json::StreamWriter> writer(writer_builder.newStreamWriter());
        writer->write(json_string, &ofile);
        ofile.close();
        DEBUG("log written to" + file_path);
        return SCM::SUCCESS;
    }

    int save(const vector<LogEvent> &alerts)
    {
        if (alerts.size() == 0)
        {
            DEBUG("no decoder matched");
            return SCM::SUCCESS;
        }
        string file_path = os::createJSONFile("log-analysis-report");
        Json::Value json;
        json["OrgId"] = 5268;
        json["Source"] = os::host_name;
        json["AppName"] = "system_events";
        json["Alerts"] = Json::Value(Json::arrayValue);
        Json::Value alert;
        Json::StreamWriterBuilder writer_builder;

        for (const auto &log : alerts)
        {
            AConfigService ConfigService = getAConfigServiceRuleByGroupAndId(log.group, log.rule_id);
            if (ConfigService.id <= 0)
            {
                DEBUG("unrecognized rule, rule_id=" + std::to_string(log.rule_id) + " RuleGroup=" + log.group);
            }
            else
            {
                AConfigService child = ConfigService;
                alert["TimeStamp"] = log.timestamp;
                alert["User"] = log.user;
                alert["Program"] = log.program;
                alert["Message"] = log.message;
                alert["Category"] = log.decoded;
                alert["MatchedRule"] = ConfigService.id;
                alert["LogLevel"] = ConfigService.level;
                while (child.if_sid > 0)
                {
                    child = getAConfigServiceRuleByGroupAndId(log.group, child.if_sid);
                }
                alert["Section"] = log.group;
                alert["Description"] = (ConfigService.description.empty()) ? child.description : ConfigService.description;
                json["Alerts"].append(alert);
            }
        }
        std::ofstream ofile(file_path); /*need update*/
        std::unique_ptr<Json::StreamWriter> writer(writer_builder.newStreamWriter());
        writer->write(json, &ofile);
        ofile.close();
        DEBUG("log written to" + file_path);
        return SCM::SUCCESS;
    }

    AConfigService getAConfigServiceRuleByGroupAndId(const string &group, const int rule_id)
    {
        AConfigService ConfigService;
        for (const auto &parent : rules)
        {
            for (const auto &child : parent.second)
            {
                if (child.first == rule_id)
                {
                    ConfigService = child.second;
                    break;
                }
            }
        }
        return ConfigService;
    }

    int printAnalysisReport(const AConfigService &rule_info, const LogEvent &log_info)
    {
        AConfigService child = rule_info;
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
            child = getAConfigServiceRuleByGroupAndId(log_info.group, child.if_sid);
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
        return SCM::SUCCESS;
    }

private:
    ConfigService configService;
    std::unordered_map<string, std::unordered_map<int, AConfigService>> rules;
    std::unordered_map<string, decoder> decoder_list;
};
#endif // IdsRepository