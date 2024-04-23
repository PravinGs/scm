#ifndef REST_SERVICE_HPP
#define REST_SERVICE_HPP
#pragma once

#include "rest_entity.hpp"

class rest_service
{
public:
    static size_t writeCallback(char *data, size_t size, size_t nmemb, std::string *response)
    {
        if (response)
        {
            response->append(data, size * nmemb);

            return size * nmemb;
        }
        return 0;
    }

    static size_t headerCallback(char *buffer, size_t size, size_t nitems, long *httpCode)
    {
        std::string header(buffer, size * nitems);
        size_t pos = header.find("HTTP/1.");
        if (pos != std::string::npos)
        {
            *httpCode = std::stoi(header.substr(pos + 9, 3));
        }
        return size * nitems;
    }

    static size_t verboseCallback(char *data, size_t size, size_t nmemb, std::string *output)
    {
        if (output)
        {
            DEBUG("Verbose Callback: ", output);
            output->append(data, size * nmemb);
            return size * nmemb;
        }
        return 0;
    }

    static string get_post_url_by_name(const rest_entity &entity, const string &type)
    {
        if (type == "auth" || type == "dpkg" || type == "applog" || type == "syslog")
        {
            return entity.logs_post_url;
        }
        else if (type == "ids")
        {
            return entity.ids_post_url;
        }
        else if (type == "patch")
        {
            return entity.patch_get_url;
        }
        else if (type == "process")
        {
            return entity.resources_post_url;
        }
        else
        {
            return "";
        }
    }

public:
    static rest_response post(const string &post_url, const string &json)
    {
        rest_response result;
        CURL *curl = nullptr;
        const char *content_type = "application/json";
        CURLcode curl_code = CURLE_OK;
        std::string response;
        long http_code = 0L;
        bool is_file = true;
        curl = curl_easy_init();
        curl_global_init(CURL_GLOBAL_DEFAULT);
        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, post_url.c_str());
            struct curl_slist *headers = nullptr;
            headers = curl_slist_append(headers, "accept: */*");

            std::string authorization_header = "Authorization: Bearer " + Audit::Rest::BEARER_TOKEN;
            headers = curl_slist_append(headers, authorization_header.c_str());

            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1L);

            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headerCallback);
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, &http_code);
            if (DEBUG_ENABLED())
            {
                curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
            }
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_code = curl_easy_perform(curl);
            if (curl_code == CURLE_OK)
            {
                result.http_code = http_code;
                result.json_response = response;
                DEBUG("HTTP Status Code: " + std::to_string(http_code));
                LOG("Response: ", response);
            }
            else
            {
                string error = curl_easy_strerror(curl_code);
                LOG_ERROR(error);
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
            curl_easy_cleanup(curl);
            curl_global_cleanup();
        }
        return result;
    }

    static void start(const rest_entity &entity, const string &name)
    {
        rest_response response;

        vector<string> json_files;

        string post_url = get_post_url_by_name(entity, name);

        string path = Audit::Config::BASE_LOG_DIR;

        int result = os::get_regular_files(path + "json/" + name, json_files);

        if (result == Audit::FAILED)
        {
            LOG_ERROR(Audit::FILE_ERROR, path);
            return;
        }
        if (post_url.empty())
        {
            LOG_ERROR("rest url should not be empty");
            return;
        }
        for (const string &json_file : json_files)
        {
            string json_string = os::read_json_file(json_file);
            response = post(post_url, json_string);
            if (!json_string.empty() && (response.http_code == Audit::Rest::POST_SUCCESS))
            {
                os::delete_file(json_file);
            }
        }
    }
};

#endif