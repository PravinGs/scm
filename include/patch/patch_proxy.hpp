#ifndef PATCH_PROXY_HPP
#define PATCH_PROXY_HPP
 
#pragma once
 
#include "patch/patch_service_impl.hpp"
#include "util/entity_parser.hpp"
 
class patch_proxy : public patch_service{
public:
	patch_proxy(): service(std::make_unique<patch_service_impl>()) {}
 
	int start(patch_entity& entity)
	{
        if (!validate_patch_entity(entity)) 
        {
            return Audit::FAILED;
        }
		return service->start(entity);
	}
	void start(config_table_type& config_table) 
	{
        int result = Audit::SUCCESS;
		patch_entity entity = parser.get_patch_entity(config_table);
        result = start(entity);
        while (result == Audit::Patch::SERVER_ERROR)
        {
            std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
            std::chrono::system_clock::time_point executionTime = currentTime + std::chrono::seconds(Audit::Patch::DOWNLOAD_WAITING_TIME);
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
private:
    bool validate_patch_entity(patch_entity& entity)
    {
        if (entity.application.empty())
        {
            LOG_ERROR("application name not configured for patch update.");
            //agent_utils::write_log("proxy: validate_patch_entity: application name not configured for patch update.", Audit::FAILED);
            return false;
        }
 
        if (entity.application_root_path.empty())
        {
            LOG_ERROR("application binary path not configured");
            //agent_utils::write_log("proxy: validate_patch_entity: application binary path not configured", Audit::FAILED);
            return false;
        }
 
        if (entity.url.empty())
        {
            LOG_ERROR("url not configured for patch download");
            //agent_utils::write_log("proxy: validate_patch_entity: url not configured for patch download", FAILED);
            return FAILED;
        }
 
        if (entity.ca_cert_path.empty())
        {
            entity.is_secure = false;
        }
        else
        {
            entity.is_secure = (os::is_file_exist(entity.ca_cert_path)) ? true : false;
        }
 
        if (entity.client_cert_path.empty())
        {
            entity.is_secure = false;
        }
        else
        {
            entity.is_secure = (os::is_file_exist(entity.client_cert_path)) ? true : false;
        }
 
        if (entity.username.empty() && entity.password.empty())
        {
            entity.is_sftp = false;
        }
 
        if (entity.retry_count == 0)
        {
            entity.retry_count =  Audit::Patch::PATCH_DEFAULT_RETRY_COUNT;
        }
 
        if (entity.retry_time_out == 0)
        {
            entity.retry_time_out =  Audit::Patch::PATCH_DEFAULT_RETRY_TIMEOUT;
        }
 
        if (entity.max_download_speed == 0)
        {
            entity.max_download_speed =  Audit::Patch::PATCH_DEFAULT_MAX_DOWNLOAD_SPEED;
        }
 
        if (entity.min_download_speed == 0)
        {
            entity.min_download_speed = Audit::Patch::PATCH_DEFAULT_MIN_DOWNLOAD_SPEED;
        }
 
        if (!os::is_dir_exist(Audit::Patch::DEFAULT_DOWNLOAD_LOCATION))
        {
            os::create_dir(Audit::Patch::DEFAULT_DOWNLOAD_LOCATION);
        }
 
        entity.download_path = Audit::Patch::DEFAULT_DOWNLOAD_LOCATION + extract_file_name_from_url(entity.url);
 
        return true;
    }
 
    string extract_file_name_from_url(const string& url)
    {
        size_t last_slash_index = url.find_last_of("/");
        if (last_slash_index != string::npos)
        {
            return url.substr(last_slash_index + 1);
        }
        return "";
    }
 
private:
	std::unique_ptr<patch_service> service = nullptr;
	entity_parser parser;
};
 
#endif //PATCH_PROXY_HPP