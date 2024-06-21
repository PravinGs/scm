#ifndef PatchEntity_HPP
#define PatchEntity_HPP
#pragma once
#include "util/Common.hpp"
 
struct PatchEntity
{
    int status;
    long size;
    int max_download_speed;
    int min_download_speed;
    int current_size;
    int expected_size;
    int retry_time_out;
    int retry_count;
    bool is_sftp;
    bool is_secure;
    string ca_cert_path;
    string client_cert_path;
    string application;
    string application_root_path;
    string download_path;
    string username;
    string password;
    string url;
    string err_msg;
    conn_entity connection;
 
    PatchEntity() : status(0), size(0L), max_download_speed(0), min_download_speed(0), retry_time_out(0), retry_count(0), is_sftp(true), is_secure(true) , current_size(0) , expected_size(0){}
};
#endif //PatchEntity_HPP