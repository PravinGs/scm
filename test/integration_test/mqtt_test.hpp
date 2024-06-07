#ifndef MQTT_CONTROLLER_INTEGRATION_TEST_HPP
#define MQTT_CONTROLLER_INTEGRATION_TEST_HPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MqttController.hpp"

struct MQTT_INTEGRATION_TEST : public testing::Test
{
    ConfigService configService;
    EntityParser entityParser;
    MqttService mqttService;
    void SetUp() { }
    void TearDown() { }
};

TEST_F(MQTT_INTEGRATION_TEST, START_MQTT_SERVICE)
{
    /*
        Input
        1. config file

        steps
        1. Read and build config table
        2. Validate config table
        3. Create Mqtt entity
        4. Validate Mqtt entity
        5. Start mqtt service 
    */
    const string configFilePath = "/home/champ/scm/config/schedule.config";
    config_table_type configTable;
    int result = SCM::SUCCESS;
    //1.Read and build the config table
    result = configService.readIniConfigFile(configFilePath, configTable);
    // 2. Validate config table
    ASSERT_EQ(result, SCM::SUCCESS); // Abbor the flow if it fails.
    // 3. Create Mqtt entity
    MqttEntity mqttEntity = entityParser.getMqttEntity(configTable);
    //  4. Validate Mqtt entity
    MqttProxy mqttProxy(configTable);
    bool validationResponse = mqttProxy.validateMqttEntity(mqttEntity);
    ASSERT_TRUE(validationResponse);
    // 5. Start mqtt service 
    result = mqttService.start();
    ASSERT_EQ(result, SCM::SUCCESS);
    //output
}

TEST_F(MQTT_INTEGRATION_TEST, INVALID_CONFIG_FILE)
{
    const string configFilePath = "/home/champ/scm/config/schedu.fig";
    config_table_type configTable;
    int result = SCM::SUCCESS;
    result = configService.readIniConfigFile(configFilePath, configTable);
    ASSERT_EQ(result, SCM::SUCCESS); 
    MqttEntity mqttEntity = entityParser.getMqttEntity(configTable);
    MqttProxy mqttProxy(configTable);
    bool validationResponse = mqttProxy.validateMqttEntity(mqttEntity);
    ASSERT_TRUE(validationResponse);
    result = mqttService.start();
    ASSERT_EQ(result, SCM::SUCCESS);
}

TEST_F(MQTT_INTEGRATION_TEST, CONNECTION_STRING_EMPTY)
{
    const string configFilePath = "/home/champ/scm/config/schedule.config";
    config_table_type configTable;
    int result = SCM::SUCCESS;
    result = configService.readIniConfigFile(configFilePath, configTable);
    ASSERT_EQ(result, SCM::SUCCESS); 
    MqttEntity mqttEntity;
    mqttEntity.conn_string = "";
    mqttEntity = entityParser.getMqttEntity(configTable);
    MqttProxy mqttProxy(configTable);
    bool validationResponse = mqttProxy.validateMqttEntity(mqttEntity);
    ASSERT_TRUE(validationResponse);
    result = mqttService.start();
    ASSERT_EQ(result, SCM::SUCCESS);
}

TEST_F(MQTT_INTEGRATION_TEST, EMPTY_TOPICS)
{
    const string configFilePath = "/home/champ/scm/config/schedule.config";
    config_table_type configTable;
    int result = SCM::SUCCESS;
    result = configService.readIniConfigFile(configFilePath, configTable);
    ASSERT_EQ(result, SCM::SUCCESS); 
    MqttEntity mqttEntity;
    mqttEntity.topics = "";
    mqttEntity = entityParser.getMqttEntity(configTable);
    MqttProxy mqttProxy(configTable);
    bool validationResponse = mqttProxy.validateMqttEntity(mqttEntity);
    ASSERT_TRUE(validationResponse);
    result = mqttService.start();
    ASSERT_EQ(result, SCM::SUCCESS);
}

TEST_F(MQTT_INTEGRATION_TEST, INVALID_CA_CERTIFICATE_PATH)
{
    const string configFilePath = "/home/champ/scm/config/schedule.config";
    config_table_type configTable;
    int result = SCM::SUCCESS;
    result = configService.readIniConfigFile(configFilePath, configTable);
    ASSERT_EQ(result, SCM::SUCCESS); 
    MqttEntity mqttEntity;
    mqttEntity.ca_cert_path = "/etc/zkeepository/keys/ca/ca_ce.em";
    mqttEntity = entityParser.getMqttEntity(configTable);
    MqttProxy mqttProxy(configTable);
    bool validationResponse = mqttProxy.validateMqttEntity(mqttEntity);
    ASSERT_TRUE(validationResponse);
    result = mqttService.start();
    ASSERT_EQ(result, SCM::SUCCESS);
}

TEST_F(MQTT_INTEGRATION_TEST, CA_CERTIFICATE_PATH_EMPTY)
{
    const string configFilePath = "/home/champ/scm/config/schedule.config";
    config_table_type configTable;
    int result = SCM::SUCCESS;
    result = configService.readIniConfigFile(configFilePath, configTable);
    ASSERT_EQ(result, SCM::SUCCESS); 
    MqttEntity mqttEntity;
    mqttEntity.ca_cert_path = "";
    mqttEntity = entityParser.getMqttEntity(configTable);
    MqttProxy mqttProxy(configTable);
    bool validationResponse = mqttProxy.validateMqttEntity(mqttEntity);
    ASSERT_TRUE(validationResponse);
    result = mqttService.start();
    ASSERT_EQ(result, SCM::SUCCESS);
}

TEST_F(MQTT_INTEGRATION_TEST, INVALID_CLIENT_CERTIFICATE_PATH)
{
    const string configFilePath = "/home/champ/scm/config/schedule.config";
    config_table_type configTable;
    int result = SCM::SUCCESS;
    result = configService.readIniConfigFile(configFilePath, configTable);
    ASSERT_EQ(result, SCM::SUCCESS); 
    MqttEntity mqttEntity;
    mqttEntity.client_cert_path = "/etc/zkey/pository/key/client/cl_cer.em";
    mqttEntity = entityParser.getMqttEntity(configTable);
    MqttProxy mqttProxy(configTable);
    bool validationResponse = mqttProxy.validateMqttEntity(mqttEntity);
    ASSERT_TRUE(validationResponse);
    result = mqttService.start();
    ASSERT_EQ(result, SCM::SUCCESS);
}

TEST_F(MQTT_INTEGRATION_TEST, CLIENT_CERTIFICATE_PATH_EMPTY)
{
    const string configFilePath = "/home/champ/scm/config/schedule.config";
    config_table_type configTable;
    int result = SCM::SUCCESS;
    result = configService.readIniConfigFile(configFilePath, configTable);
    ASSERT_EQ(result, SCM::SUCCESS); 
    MqttEntity mqttEntity;
    mqttEntity.client_cert_path = "";
    mqttEntity = entityParser.getMqttEntity(configTable);
    MqttProxy mqttProxy(configTable);
    bool validationResponse = mqttProxy.validateMqttEntity(mqttEntity);
    ASSERT_TRUE(validationResponse);
    result = mqttService.start();
    ASSERT_EQ(result, SCM::SUCCESS);
}

#endif