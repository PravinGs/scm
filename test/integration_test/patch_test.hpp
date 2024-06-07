#ifndef PATCH_CONTROLLER_INTEGRATION_TEST_HPP
#define PATCH_CONTROLLER_INTEGRATION_TEST_HPP

#include <gtest/gtest.h>
#include "patch/PatchController.hpp"

struct PATCH_INTEGRATION_TEST : public testing::Test
{
    ConfigService configService;
    EntityParser entityParser;
    PatchServiceImpl patchService;
    void SetUp() { }
    void TearDown() { }
};

TEST_F(PATCH_INTEGRATION_TEST, START_PATCH_SERVICE)
{
    /*
        Input
        1. config file

        steps
        1. Read and build config table
        2. Validate config table
        3. Create Patch entity
        4. Validate Patch entity
        5. Update to the latest version
    */
    const string configFilePath = "/home/champ/scm/config/schedule.config";
    config_table_type configTable;
    int result = SCM::SUCCESS;
    //1.Read and build the config table
    result = configService.readIniConfigFile(configFilePath, configTable);
    // 2. Validate config table
    ASSERT_EQ(result, SCM::SUCCESS); // Abbor the flow if it fails.
    // 3. Create Patch entity
    PatchEntity patchEntity = entityParser.getPatchEntity(configTable);
    //  4. Validate Patch entity
    PatchProxy patchProxy(configTable);
    bool validationResponse = patchProxy.validatePatchEntity(patchEntity);
    ASSERT_TRUE(validationResponse);
    // 5. Update to the latest version
    result = patchService.start(patchEntity);
    ASSERT_EQ(result, SCM::SUCCESS);
    //output
}

TEST_F(PATCH_INTEGRATION_TEST, INVALID_CONFIG_FILE)
{
    const string configFilePath = "/home/champ/scfig/schedule.config";
    config_table_type configTable;
    int result = SCM::SUCCESS;
    result = configService.readIniConfigFile(configFilePath, configTable);
    ASSERT_EQ(result, SCM::SUCCESS); 
    PatchEntity patchEntity = entityParser.getPatchEntity(configTable);
    PatchProxy patchProxy(configTable);
    bool validationResponse = patchProxy.validatePatchEntity(patchEntity);
    ASSERT_TRUE(validationResponse);
    result = patchService.start(patchEntity);
    ASSERT_EQ(result, SCM::SUCCESS);
}

TEST_F(PATCH_INTEGRATION_TEST, APPLICATION_NAME_EMPTY)
{
    const string configFilePath = "/home/champ/scm/config/schedule.config";
    config_table_type configTable;
    int result = SCM::SUCCESS;
    result = configService.readIniConfigFile(configFilePath, configTable);
    ASSERT_EQ(result, SCM::SUCCESS); 
    PatchEntity patchEntity;
    patchEntity.application = "";
    patchEntity = entityParser.getPatchEntity(configTable);
    PatchProxy patchProxy(configTable);
    bool validationResponse = patchProxy.validatePatchEntity(patchEntity);
    ASSERT_TRUE(validationResponse);
    result = patchService.start(patchEntity);
    ASSERT_EQ(result, SCM::SUCCESS);
}

TEST_F(PATCH_INTEGRATION_TEST, APPLICATION_ROOTPATH_EMPTY)
{
    const string configFilePath = "/home/champ/scm/config/schedule.config";
    config_table_type configTable;
    int result = SCM::SUCCESS;
    result = configService.readIniConfigFile(configFilePath, configTable);
    ASSERT_EQ(result, SCM::SUCCESS); 
    PatchEntity patchEntity;
    patchEntity.application_root_path = "";
    patchEntity = entityParser.getPatchEntity(configTable);
    PatchProxy patchProxy(configTable);
    bool validationResponse = patchProxy.validatePatchEntity(patchEntity);
    ASSERT_TRUE(validationResponse);
    result = patchService.start(patchEntity);
    ASSERT_EQ(result, SCM::SUCCESS);
}

TEST_F(PATCH_INTEGRATION_TEST, URL_EMPTY)
{
    const string configFilePath = "/home/champ/scm/config/schedule.config";
    config_table_type configTable;
    int result = SCM::SUCCESS;
    result = configService.readIniConfigFile(configFilePath, configTable);
    ASSERT_EQ(result, SCM::SUCCESS); 
    PatchEntity patchEntity;
    patchEntity.url = "";
    patchEntity = entityParser.getPatchEntity(configTable);
    PatchProxy patchProxy(configTable);
    bool validationResponse = patchProxy.validatePatchEntity(patchEntity);
    ASSERT_TRUE(validationResponse);
    result = patchService.start(patchEntity);
    ASSERT_EQ(result, SCM::SUCCESS);
}

#endif