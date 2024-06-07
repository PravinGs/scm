#ifndef IDS_CONTROLLER_INTEGRATION_TEST_HPP
#define IDS_CONTROLLER_INTEGRATION_TEST_HPP

#include <gtest/gtest.h>
#include "ids/IdsController.hpp"

struct IDS_INTEGRATION_TEST : public testing::Test
{
    ConfigService configService;
    EntityParser entityParser;
    IdsServiceImpl idsService;
    IdsRepository idsRepository;
    void SetUp() { }
    void TearDown() { }
};

TEST_F(IDS_INTEGRATION_TEST, ANALYSE_FILE)
{
    /*
        Input
        1. config file

        steps
        1. Read and build config table
        2. Validate config table
        3. Create Analysis entity
        4. Validate Analysis entity
        5. Analyse the files
        6. Write and save the ids data to JSON file
        output
        1.log file
    */
    const string configFilePath = "/home/champ/scm/config/schedule.config";
    config_table_type configTable;
    int result = SCM::SUCCESS;
    //1.Read and build the config table
    result = configService.readIniConfigFile(configFilePath, configTable);
    // 2. Validate config table
    ASSERT_EQ(result, SCM::SUCCESS); // Abbor the flow if it fails.
    // 3. Create Analysis entity
    AnalysisEntity analysisEntity = entityParser.getAnalysisEntity(configTable);
    // 4. Validate Analysis entity
    IdsProxy idsProxy(configTable);
    bool validationResponse = idsProxy.validateAnalysisEntity(analysisEntity);
    ASSERT_TRUE(validationResponse);
    // 5. Analyse the files
    const string file = "/home/champ/ids_test";
    result = idsService.analyseFile(file);
    // result = idsService.start(analysisEntity);
    ASSERT_EQ(result, SCM::SUCCESS);
    //6. Write and save the ids data to JSON file
    vector<LogEvent> alerts;
    result = idsRepository.save(alerts);
    ASSERT_EQ(result, SCM::SUCCESS); 
    //output
}

// TEST_F(IDS_INTEGRATION_TEST, INVALID_CONFIG_FILE)
// {
//     const string configFilePath = "/home/champ/scm/conficonfig";
//     config_table_type configTable;
//     int result = SCM::SUCCESS;
//     result = configService.readIniConfigFile(configFilePath, configTable);
//     ASSERT_EQ(result, SCM::SUCCESS); 
//     AnalysisEntity analysisEntity = entityParser.getAnalysisEntity(configTable);
//     IdsProxy idsProxy(configTable);
//     bool validationResponse = idsProxy.validateAnalysisEntity(analysisEntity);
//     ASSERT_TRUE(validationResponse);
//     const string file = "/home/champ/ids_test";
//     result = idsService.analyseFile(file);
    // result = idsService.start(analysisEntity);
//     ASSERT_EQ(result, SCM::SUCCESS);
//     vector<LogEvent> alerts;
//     result = idsRepository.save(alerts);
//     ASSERT_EQ(result, SCM::SUCCESS); 
// }

TEST_F(IDS_INTEGRATION_TEST, INVALID_DECODER_PATH)
{
    const string configFilePath = "/home/champ/scm/config/schedule.config";
    config_table_type configTable;
    int result = SCM::SUCCESS;
    result = configService.readIniConfigFile(configFilePath, configTable);
    ASSERT_EQ(result, SCM::SUCCESS); 
    AnalysisEntity analysisEntity;
    analysisEntity.decoder_path = "/etc/scl/decoder/dec"
    analysisEntity = entityParser.getAnalysisEntity(configTable);
    IdsProxy idsProxy(configTable);
    bool validationResponse = idsProxy.validateAnalysisEntity(analysisEntity);
    ASSERT_TRUE(validationResponse);
    const string file = "/home/champ/ids_test";
    result = idsService.analyseFile(file);
    // result = idsService.start(analysisEntity);
    ASSERT_EQ(result, SCM::SUCCESS);
    vector<LogEvent> alerts;
    result = idsRepository.save(alerts);
    ASSERT_EQ(result, SCM::SUCCESS); 
}

TEST_F(IDS_INTEGRATION_TEST, INVALID_RULES_PATH)
{
    const string configFilePath = "/home/champ/scm/config/schedule.config";
    config_table_type configTable;
    int result = SCM::SUCCESS;
    result = configService.readIniConfigFile(configFilePath, configTable);
    ASSERT_EQ(result, SCM::SUCCESS); 
    AnalysisEntity analysisEntity;
    analysisEntity.rules_path = "/home/champ/scm/rules/sysrulxml"
    analysisEntity = entityParser.getAnalysisEntity(configTable);
    IdsProxy idsProxy(configTable);
    bool validationResponse = idsProxy.validateAnalysisEntity(analysisEntity);
    ASSERT_TRUE(validationResponse);
    const string file = "/home/champ/ids_test";
    result = idsService.analyseFile(file);
    // result = idsService.start(analysisEntity);
    ASSERT_EQ(result, SCM::SUCCESS);
    vector<LogEvent> alerts;
    result = idsRepository.save(alerts);
    ASSERT_EQ(result, SCM::SUCCESS); 
}

/*
TEST_F(IDS_INTEGRATION_TEST, INVALID_FILE_PATH)
{
    const string configFilePath = "/home/champ/scm/config/schedule.config";
    config_table_type configTable;
    int result = SCM::SUCCESS;
    result = configService.readIniConfigFile(configFilePath, configTable);
    ASSERT_EQ(result, SCM::SUCCESS); 
    AnalysisEntity analysisEntity = entityParser.getAnalysisEntity(configTable);
    IdsProxy idsProxy(configTable);
    bool validationResponse = idsProxy.validateAnalysisEntity(analysisEntity);
    ASSERT_TRUE(validationResponse);
    const string file = "/home/champids_test";
    result = idsService.analyseFile(file);
    ASSERT_EQ(result, SCM::SUCCESS);
    vector<LogEvent> alerts;
    result = idsRepository.save(alerts);
    ASSERT_EQ(result, SCM::SUCCESS); 
}

TEST_F(IDS_INTEGRATION_TEST, EMPTY_FILE)
{
    const string configFilePath = "/home/champ/scm/config/schedule.config";
    config_table_type configTable;
    int result = SCM::SUCCESS;
    result = configService.readIniConfigFile(configFilePath, configTable);
    ASSERT_EQ(result, SCM::SUCCESS); 
    AnalysisEntity analysisEntity = entityParser.getAnalysisEntity(configTable);
    IdsProxy idsProxy(configTable);
    bool validationResponse = idsProxy.validateAnalysisEntity(analysisEntity);
    ASSERT_TRUE(validationResponse);
    const string file = "";
    result = idsService.analyseFile(file);
    ASSERT_EQ(result, SCM::SUCCESS);
    vector<LogEvent> alerts;
    result = idsRepository.save(alerts);
    ASSERT_EQ(result, SCM::SUCCESS); 
}
*/
#endif