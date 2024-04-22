#ifndef PATCH_UNIT_TEST_HPP
#define PATCH_UNIT_TEST_HPP
#pragma once
 
#include "patch/patch_proxy.hpp"
#include <gtest/gtest.h>  
 
struct PATCH_TEST : public testing::Test
{
    patch_proxy patch;
    void setup() {}
    void TearDown() {}
};
 
 
TEST_F(PATCH_TEST , ValidPatchEntity)
{
    patch_entity entity;
    entity.size ;
    entity.max_download_speed = 4096;
    entity.min_download_speed = 1024 ;
    entity.retry_time_out = 30;
    entity.retry_count = 5;
    entity.is_sftp = true; 
    entity.is_secure = true;
    entity.ca_cert_path =" ";  
    entity.client_cert_path =" ";
    entity.application = "agent" ;
    entity.application_root_path = "/etc/scl/bin/" ;
    entity.download_path = " ";
    entity.username = "demo"; 

    entity.password = "password";
    entity.url = "sftp://test.rebex.net/pub/example/readme.txt";
   
    bool response = patch.validate_patch_entity(entity);
    EXPECT_TRUE(response);
}


#endif