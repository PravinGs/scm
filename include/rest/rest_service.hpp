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

            output->append(data, size * nmemb);

            return size * nmemb;
        }

        return 0;
    }

    static string get_post_url_by_name(const rest_entity &entity, const string &type)

    {

        if (type == "log")

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

        else if (type == "resource")

        {

            return entity.resources_post_url;
        }

        else

        {

            return "";
        }
    }

public:
    static long post(rest_entity entity, const string &data)

    {

        return post(entity.logs_post_url, entity.attribute_name, data, false);
    }

    static long post(const string &post_url, const string &attribute_name, const string &json, bool is_file_type)

    {

        CURL *curl = nullptr;

        const char *content_type = "application/json";

        CURLcode curl_code = CURLE_OK;

        std::string response;

        long http_code = 0L;

        bool is_file = true;

        if (is_file_type)

        {

            is_file = os::is_file_exist(json);

            if (!is_file)

                return 0L;
        }

        curl = curl_easy_init();

        curl_global_init(CURL_GLOBAL_DEFAULT);

        if (curl)

        {

            curl_easy_setopt(curl, CURLOPT_URL, post_url.c_str());

            struct curl_slist *headers = nullptr;

            headers = curl_slist_append(headers, "accept: */*");

            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1L);

            if (is_file_type)

            {

                struct curl_httppost *form = nullptr;

                struct curl_httppost *last = nullptr;

                curl_formadd(&form, &last, CURLFORM_COPYNAME, attribute_name.c_str(), CURLFORM_FILE, json.c_str(), CURLFORM_CONTENTTYPE, content_type, CURLFORM_END);

                curl_easy_setopt(curl, CURLOPT_HTTPPOST, form);
            }

            else
            {

                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
            }

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);

            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headerCallback);

            curl_easy_setopt(curl, CURLOPT_HEADERDATA, &http_code);

            // Enable verbose output

            // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

            curl_code = curl_easy_perform(curl);

            if (curl_code == CURLE_OK)

            {

                common::write_log("rest_service: post: request successful", Audit::DEBUG);

                common::write_log("rest_service: post: HTTP Status Code: " + std::to_string(http_code), Audit::SUCCESS);

                std::cout << "Response: " << response << "\n";
            }

            else

            {

                string error = curl_easy_strerror(curl_code);

                common::write_log("rest_service: post: request Audit::FAILED: " + error, Audit::FAILED);
            }

            if (http_code == Audit::Rest::POST_SUCCESS && is_file_type)

            {

                os::delete_file(json);
            }

            else if (http_code != Audit::Rest::POST_SUCCESS && is_file_type)

            {

                common::write_log("rest_service: post: Audit::FAILED to send this file " + json, Audit::FAILED);
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));

            curl_easy_cleanup(curl);

            curl_global_cleanup();
        }

        return http_code;
    }

    static int start(const rest_entity &entity, const string &name)

    {

        long http_code = 0L;

        vector<string> json_files;

        string post_url = get_post_url_by_name(entity, name);

        string path = Audit::Config::BASE_LOG_DIR;

        int result = os::get_regular_files(name + "json/", json_files);

        if (result == Audit::FAILED)

        {

            common::write_log("rest_service: start: " + Audit::FILE_ERROR + path, Audit::FAILED);

            return Audit::FAILED;
        }

        if (post_url.empty())

        {

            common::write_log("rest_service: start: invalid name given", Audit::FAILED);

            return Audit::FAILED;
        }

        if (entity.attribute_name.empty())

        {

            common::write_log("rest_service: start: invalid attribute name given", Audit::FAILED);

            return Audit::FAILED;
        }

        for (const string &json_file : json_files)

        {

            http_code = post(post_url, entity.attribute_name, json_file, true);
        }

        return http_code;
    }
};

#endif