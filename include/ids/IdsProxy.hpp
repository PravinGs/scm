#ifndef IdsProxy_HPP
#define IdsProxy_HPP
#pragma once 
#include "ids/IdsServiceImpl.hpp"
#include "util/EntityParser.hpp"
 
class IdsProxy: public IdsService {
public:
    IdsProxy() : service(std::make_unique<IdsServiceImpl>()) {}

    int rootkitAnalysis()
    {
        return service->rootkitAnalysis();
    }
 
	int start(AnalysisEntity& entity) {
        if (!validateAnalysisEntity(entity))
        {
            return SCM::FAILED;
        }
        if (entity.time_pattern.empty())
        {
            return service->start(entity);
        }
        // need to update
        return service->start(entity);
	}
	int start(config_table_type& config_table) 
    {
        AnalysisEntity entity = entity_parser.getAnalysisEntity(config_table);
        return start(entity);
	}
private:
    bool validateAnalysisEntity(AnalysisEntity& entity)
    {
        if (!entity.log_path.empty() && !os::isFileExist(entity.log_path))
        {
            LOG_ERROR(SCM::FILE_ERROR + entity.log_path);
            return false;
        }
 
        if (!entity.decoder_path.empty() && !os::isFileExist(entity.decoder_path))
        {
            LOG_ERROR(SCM::FILE_ERROR + entity.decoder_path);
            return false;
        }
        else
        {
            entity.decoder_path = SCM::IDS::IDS_DEFAULT_DECODER_RULES_PATH;
        }
 
        if (!entity.write_path.empty() && !os::isDirExist(entity.write_path))
        {
            DEBUG(SCM::FILE_ERROR + entity.write_path);
        }
 
        if (!entity.rules_path.empty() && !os::isDirExist(entity.rules_path))
        {
            LOG_ERROR(SCM::FILE_ERROR + entity.rules_path);
            return false;
        }
        else
        {
            entity.rules_path = SCM::IDS::IDS_DEFAULT_XML_RULES_PATH;
        }
 
        if (entity.storage_type.empty())
        {
            entity.storage_type = "json";
        }
        return true;
    }
private:
	EntityParser entity_parser;
    std::unique_ptr<IdsService> service = nullptr;
};
 
#endif // !IdsProxy_HPP