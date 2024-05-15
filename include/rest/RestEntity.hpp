#ifndef RestEntity_HPP
#define RestEntity_HPP
#include "util/Common.hpp"

struct RestEntity
{
    string access_token_url;
    string logs_post_url;
    string ids_post_url;
    string patch_get_url;
    string resources_post_url;
    string attribute_name;
    string ca_cert_path;
    string client_cert_path;
    conn_entity connection;
};

struct RestResponse 
{
    long http_code;
    string json_response;
    RestResponse():http_code(-1L){}
};

#endif 