#ifndef PATCH_SERVICE_IMPL_HPP
#define PATCH_SERVICE_IMPL_HPP
#pragma once
#include "patch/patch_service.hpp"
class patch_service_impl : public patch_service {
public:
	int start(patch_entity& entity) 
    {
        int result = Audit::SUCCESS;
        int count = entity.retry_count;
        curl = curl_easy_init();
        while (count > 0)
        {
            result = download(entity);
 
            if (result == Audit::SUCCESS)
            {
                DEBUG("Download completed..");
                //agent_utils::write_log("patch_service: start: download completed..", SUCCESS);
                break;
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::seconds(entity.retry_time_out));
                count--;
            }
        }
        if (curl != nullptr)
        {
            curl_easy_cleanup(curl);
        }
        if (count <= 0)
        {
            result = Audit::SCHEDULAR_WAIT;
            DEBUG("reached time limit exiting from download");
            //agent_utils::write_log("patch_service: start: reached time limit exiting from download", WARNING);
        }
        return result;
	}
 
private:
    int download(patch_entity& entity)
    {
        int return_val = Audit::SUCCESS;
        int retry = entity.retry_time_out;
        FILE* file = NULL;
        CURLcode res;
 
        file = fopen(entity.download_path.c_str(), "ab");
        if (file == NULL)
        {
            LOG_ERROR(Audit::FILE_ERROR + entity.download_path);
            //agent_utils::write_log("patch_service: download: failed to open " + entity.download_path + " check file path and it's permission", FAILED);
            return Audit::FAILED;
        }
 
        entity.size = common::get_file_size(entity.download_path);
 
        if (curl == NULL)
        {
            LOG_ERROR("failed to initialize curl");
            //agent_utils::write_log("patch_service: download: failed to initialize curl ", FAILED);
            return Audit::FAILED;
        }
 
        curl_easy_setopt(curl, CURLOPT_URL, entity.url.c_str());
        curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, entity.size);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
 
        if (entity.is_sftp)
        {
            string credential = entity.username + ":" + entity.password;
            curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PASSWORD);
            curl_easy_setopt(curl, CURLOPT_USERPWD, credential.c_str());
        }
 
        if (!entity.is_secure)
        {
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        }
        else
        {
            curl_easy_setopt(curl, CURLOPT_CAINFO, entity.ca_cert_path.c_str());
            if (!entity.client_cert_path.empty())
            {
                curl_easy_setopt(curl, CURLOPT_SSLCERT, entity.client_cert_path.c_str());
            }
        }
 
        curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, entity.min_download_speed);
        curl_easy_setopt(curl, CURLOPT_MAX_RECV_SPEED_LARGE, entity.max_download_speed);
        curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, entity.retry_time_out);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, entity.retry_time_out);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
        do
        {
            res = curl_easy_perform(curl);
 
            if (res != CURLE_OK)
            {
                string error = curl_easy_strerror(res);
                LOG_ERROR(error);
               // agent_utils::write_log("patch_service: download: " + error, FAILED);
                //std::cerr << "Error: " << error << "\n";
                if (res == CURLE_COULDNT_RESOLVE_HOST)
                {
                    return_val = FAILED;
                    break;
                }
                retry--;
                if (retry > 0)
                {
                    std::cerr << "Retrying download in 10 seconds...\n";
                    LOG("Retrying download in 10 seconds...");
                    //agent_utils::write_log("patch_service: download: retrying download in 10 seconds...");
                    std::this_thread::sleep_for(std::chrono::seconds(19));
                }
                else
                {
                    return_val = Audit::Patch::SERVER_ERROR;
                }
            }
            else
            {
                retry = entity.retry_count;
            }
        } while (res != CURLE_OK && retry > 0);
        if (retry <= 0)
        {
            return_val = Audit::FAILED;
        }
        fclose(file);
        return return_val;
    }

private:
    CURL* curl = nullptr;
};
 
#endif //PATCH_SERVICE_IMPL_HPP