#ifndef PatchEntity_HPP
#define PatchEntity_HPP
#pragma once
#include "util/Common.hpp"
 
struct PatchEntity
{
    long size;
    int max_download_speed;
    int min_download_speed;
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
    conn_entity connection;
 
    PatchEntity() : size(0L), max_download_speed(0), min_download_speed(0), retry_time_out(0), retry_count(0), is_sftp(true), is_secure(true) {}
};
#endif //PatchEntity_HPP