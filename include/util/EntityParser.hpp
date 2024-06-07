#ifndef ENTITY_PARSER_HPP
#define ENTITY_PARSER_HPP
#pragma once

#include "log_collector/LogEntity.hpp"
#include "monitor/MonitorEntity.hpp"
#include "ids/IdsEntity.hpp"
#include "rest/RestEntity.hpp"
#include "mqtt/MqttEntity.hpp"
#include "patch/PatchEntity.hpp"
class EntityParser
{
public:
    LogEntity getLogEntity(config_table_type &config_table, const string &name)
    {
        LogEntity entity;
        entity.name = name;
        entity.read_path = Common::trim(config_table[name]["read_path"]);
        entity.write_path = Common::trim(config_table[name]["write_path"]); // Not a madatory field
        entity.time_pattern = Common::trim(config_table[name]["time_pattern"]); // Not a mandatory field but also it shouldn't be incorrect(follow cron expression)
        entity.storage_type = Common::trim(config_table[name]["storage_type"]);
        entity.connection = getConnEntity(config_table, "client");
        string format = Common::trim(config_table[name]["format"]);
        if (format.empty())
        {
            format = name;
        }
        entity.format = format;
        string json_attributes = Common::trim(config_table[name]["json_attributes"]);
        if (!json_attributes.empty())
        {
            entity.json_attributes = Common::toVector(json_attributes, ',');
        }
        else
        {
            entity.json_attributes.clear();
        }

        string log_levels = Common::trim(config_table[name]["log_levels"]);
        if (!log_levels.empty())
        {
            entity.log_levels = Common::toVector(log_levels, ',');
        }
        else
        {
            entity.log_levels.clear();
        }
        string delimeter = Common::trim(config_table[name]["delimeter"]);
        if (delimeter.empty())
        {
            delimeter = " ";
        }
        entity.delimeter = delimeter[0];
        string remote = Common::trim(config_table[name]["remote"]);
        if (remote.empty())
        {
            remote = "N";
        }
        entity.remote = remote[0];

        return entity;
    }

    ProcessEntity getProcessEntity(map<string, map<string, string>> &config_table)
    {
        ProcessEntity entity;
        entity.write_path = Common::trim(config_table["process"]["write_path"]);
        entity.time_pattern = Common::trim(config_table["process"]["time_pattern"]);
        entity.storage_type = Common::trim(config_table["process"]["storage_type"]);
        entity.connection = getConnEntity(config_table, "client");
        return entity;
    }

    conn_entity getConnEntity(map<string, map<string, string>> &config_table, const string &type)
    {
        conn_entity entity;
        string cert_pem = type + "_cert_path";
        string key_pem = type + "_private_key_path";
        string port;
        entity.ca_pem = Common::trim(config_table["connection"]["ca_cert_path"]);
        entity.cert_pem = Common::trim(config_table["connection"][cert_pem]);
        entity.key_pem = Common::trim(config_table["connection"][key_pem]);
        port = Common::trim(config_table["connection"]["port"]);
        try
        {
            entity.port = std::stoi(port);
        }
        catch (const std::exception &e)
        {
            string error = e.what();
            LOG_ERROR(error);
            // agent_utils::writeLog("entity_parser: getConnEntity: " + error, FAILED);
        }

        if (type == "client")
        {
            entity.conn_string = Common::trim(config_table["connection"]["host_address"]) + ":" + port;
        }
        return entity;
    }

    AnalysisEntity getAnalysisEntity(map<string, map<string, string>> &config_table)
    {
        AnalysisEntity entity;
        entity.log_path = Common::trim(config_table["analysis"]["log_path"]);
        entity.decoder_path = Common::trim(config_table["analysis"]["decoder_path"]);
        entity.rules_path = Common::trim(config_table["analysis"]["rules_path"]);
        entity.write_path = Common::trim(config_table["analysis"]["write_path"]);
        entity.time_pattern = Common::trim(config_table["analysis"]["time_pattern"]);
        entity.storage_type = Common::trim(config_table["analysis"]["storage_type"]);
        entity.connection = getConnEntity(config_table, "client");
        return entity;
    }

    PatchEntity getPatchEntity(map<string, map<string, string>> &config_table)
    {
        PatchEntity entity;
        entity.application = Common::trim(config_table["firmware"]["application"]);
        entity.application_root_path = Common::trim(config_table["firmware"]["application_root_path"]);
        entity.username = Common::trim(config_table["firmware"]["username"]);
        entity.password = Common::trim(config_table["firmware"]["password"]);
        entity.url = Common::trim(config_table["firmware"]["url"]);
        entity.ca_cert_path = Common::trim(config_table["firmware"]["ca_cert_path"]);
        entity.connection = getConnEntity(config_table, "client");
        entity.client_cert_path = Common::trim(config_table["firmware"]["client_cert_path"]);
        try
        {
            entity.max_download_speed = std::stoi(Common::trim(config_table["firmware"]["max_download_speed"]));
            entity.min_download_speed = std::stoi(Common::trim(config_table["firmware"]["min_download_speed"]));
            entity.retry_time_out = std::stoi(Common::trim(config_table["firmware"]["retry_time_out"]));
            entity.retry_count = std::stoi(Common::trim(config_table["firmware"]["retry_count"]));
        }
        catch (const std::exception &e)
        {
            // string error = e.what();
            DEBUG(e.what());
            // agent_utils::writeLog("patch_entitty getPatchEntity() exception occured: " + error, FAILED);
        }

        return entity;
    }

    MqttEntity getMqttEntity(map<string, map<string, string>> &config_table)
    {
        MqttEntity entity;
        entity.client_id = config_table["mqtt"]["client_id"];
        entity.conn_string = config_table["mqtt"]["address"];
        entity.topics = Common::toVector(config_table["mqtt"]["topics"], ',');
        entity.ca_cert_path = config_table["mqtt"]["ca_cert_path"];
        entity.client_cert_path = config_table["mqtt"]["client_cert_path"];
        try
        {
            entity.port = std::stoi(config_table["mqtt"]["port"]);
        }
        catch (const std::exception &e)
        {
            entity.port = 8000;
            std::cerr << e.what() << '\n';
            LOG_ERROR(e.what());
        }

        return entity;
    }

    RestEntity getRestEntity(map<string, map<string, string>> &config_table)
    {
        RestEntity entity;
        entity.access_token_url = config_table["cloud"]["access_token_url"];
        entity.logs_post_url = config_table["cloud"]["logs_post_url"];
        entity.ids_post_url = config_table["cloud"]["ids_post_url"];
        entity.resources_post_url = config_table["cloud"]["resources_post_url"];
        entity.ca_cert_path = config_table["cloud"]["ca_cert_path"];
        entity.client_cert_path = config_table["cloud"]["client_cert_path"];
        entity.connection = getConnEntity(config_table, "client");
        return entity;
    }

};

#endif // !ENTITY_PARSER_HPP