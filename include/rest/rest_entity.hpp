#ifndef REST_ENTITY_HPP
#define REST_ENTITY_HPP
#include "util/common.hpp"

struct rest_entity
{
    string logs_post_url;
    string ids_post_url;
    string patch_get_url;
    string resources_post_url;
    string attribute_name;
    string ca_cert_path;
    string client_cert_path;
    conn_entity connection;
};

struct rest_response 
{
    long http_code;
    string json_response;
    rest_response():http_code(0L){}
};

#endif 