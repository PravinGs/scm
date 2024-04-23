#ifndef IDS_PROXY_HPP
#define IDS_PROXY_HPP
#pragma once 
#include "ids/ids_service_impl.hpp"
#include "util/entity_parser.hpp"
 
class ids_proxy: public ids_service {
public:
    ids_proxy() : service(std::make_unique<ids_service_impl>()) {}

    int rootkit_analysis()
    {
        return service->rootkit_analysis();
    }
 
	int start(analysis_entity& entity) {
        // if (!validate_analysis_entity(entity))
        // {
        //     return Audit::FAILED;
        // }
        // if (entity.time_pattern.empty())
        // {
        //     return service->start(entity);
        // }
        // // need to update
        // return service->start(entity);
        return rootkit_analysis();
	}
	int start(config_table_type& config_table) 
    {
        analysis_entity entity = parser.get_analysis_entity(config_table);
        return start(entity);
	}
private:
    bool validate_analysis_entity(analysis_entity& entity)
    {
        if (!entity.log_path.empty() && !os::is_file_exist(entity.log_path))
        {
            LOG_ERROR(Audit::FILE_ERROR + entity.log_path);
            //agent_utils::write_log("proxy: validate_analysis_entity: invalid log path configured log analysis: " + entity.log_path, FAILED);
            return false;
        }
 
        if (!entity.decoder_path.empty() && !os::is_file_exist(entity.decoder_path))
        {
            LOG_ERROR(Audit::FILE_ERROR + entity.decoder_path);
            //agent_utils::write_log("proxy: validate_analysis_entity: invalid decoder path configured: " + entity.decoder_path, FAILED);
            return false;
        }
        else
        {
            entity.decoder_path = Audit::IDS::IDS_DEFAULT_DECODER_RULES_PATH;
        }
 
        if (!entity.write_path.empty() && !os::is_dir_exist(entity.write_path))
        {
            DEBUG(Audit::FILE_ERROR + entity.write_path);
            //agent_utils::write_log("proxy: validate_analysis_entity: configured analyis write path not exist default path set", WARNING);
        }
 
        if (!entity.rules_path.empty() && !os::is_dir_exist(entity.rules_path))
        {
            LOG_ERROR(Audit::FILE_ERROR + entity.rules_path);
            //agent_utils::write_log("proxy: validate_analysis_entity: invalid xml-rules path configured: " + entity.rules_path, FAILED);
            return false;
        }
        else
        {
            entity.rules_path = Audit::IDS::IDS_DEFAULT_XML_RULES_PATH;
        }
 
        if (entity.storage_type.empty())
        {
            entity.storage_type = "json";
        }
        return true;
    }
private:
	entity_parser parser;
    std::unique_ptr<ids_service> service = nullptr;
};
 
#endif // !IDS_PROXY_HPP