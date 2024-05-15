#ifndef PatchProxy_HPP
#define PatchProxy_HPP
 
#pragma once
 
#include "patch/PatchServiceImpl.hpp"
#include "util/entity_parser.hpp"
 
class PatchProxy : public PatchService{
public:
	PatchProxy(): service(std::make_unique<PatchServiceImpl>()) {}
 
	int start(PatchEntity& entity)
	{
        if (!validatePatchEntity(entity)) 
        {
            return SCM::FAILED;
        }
		return service->start(entity);
	}
	void start(config_table_type& config_table) 
	{
        int result = SCM::SUCCESS;
		PatchEntity entity = parser.getPatchEntity(config_table);
        result = start(entity);
        while (result == SCM::Patch::SERVER_ERROR)
        {
            std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
            std::chrono::system_clock::time_point executionTime = currentTime + std::chrono::seconds(SCM::Patch::DOWNLOAD_WAITING_TIME);
            std::chrono::system_clock::duration duration = executionTime - currentTime;
            int waitingTime = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
            std::this_thread::sleep_for(std::chrono::seconds(waitingTime));
            result = start(entity);
        }
 
        /*if (result == SUCCESS && client_connection == SUCCESS)
        {
            client_data data = connection::build_client_data("patch", "publish");
            client.send_client_data(data);
        }*/
        return;
	}
// private:
    bool validatePatchEntity(PatchEntity& entity)
    {
        if (entity.application.empty())
        {
            LOG_ERROR("application name not ConfigServiceured for patch update.");
            return false;
        }
 
        if (entity.application_root_path.empty())
        {
            LOG_ERROR("application binary path not ConfigServiceured");
            return false;
        }
 
        if (entity.url.empty())
        {
            LOG_ERROR("url not ConfigServiceured for patch download");
            return SCM::FAILED; 
        }
 
        if (entity.ca_cert_path.empty())
        {
            entity.is_secure = false;
        }
        else
        {
            entity.is_secure = os::isFileExist(entity.ca_cert_path);
        }
 
        if (entity.client_cert_path.empty())
        {
            entity.is_secure = false;
        }
        else
        {
            entity.is_secure = os::isFileExist(entity.client_cert_path);
        }
 
        if (entity.username.empty() && entity.password.empty())
        {
            entity.is_sftp = false;
        }
 
        if (entity.retry_count == 0)
        {
            entity.retry_count =  SCM::Patch::PATCH_DEFAULT_RETRY_COUNT;
        }
 
        if (entity.retry_time_out == 0)
        {
            entity.retry_time_out =  SCM::Patch::PATCH_DEFAULT_RETRY_TIMEOUT;
        }
 
        if (entity.max_download_speed == 0)
        {
            entity.max_download_speed =  SCM::Patch::PATCH_DEFAULT_MAX_DOWNLOAD_SPEED;
        }
 
        if (entity.min_download_speed == 0)
        {
            entity.min_download_speed = SCM::Patch::PATCH_DEFAULT_MIN_DOWNLOAD_SPEED;
        }
 
        if (!os::isDirExist(SCM::Patch::DEFAULT_DOWNLOAD_LOCATION))
        {
            os::createDir(SCM::Patch::DEFAULT_DOWNLOAD_LOCATION);
        }
 
        entity.download_path = SCM::Patch::DEFAULT_DOWNLOAD_LOCATION + extractFileNameFromUrl(entity.url);
 
        return true;
    }
 
    string extractFileNameFromUrl(const string& url)
    {
        size_t last_slash_index = url.find_last_of("/");
        if (last_slash_index != string::npos)
        {
            return url.substr(last_slash_index + 1);
        }
        return "";
    }
 
private:
	std::unique_ptr<PatchService> service = nullptr;
	entity_parser parser;
};
 
#endif //PatchProxy_HPP