#ifndef MQTT_CONTROLLER_INTEGRATION_TEST_HPP
#define MQTT_CONTROLLER_INTEGRATION_TEST_HPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MqttController.hpp"

class MockMqttClient : public MqttClient 
{
public:
    MOCK_METHOD(void, start, (), (override));
};

class MqttIntegrationTest : public ::testing::Test 
{
protected:
    void SetUp() override {
        mockMqttClient = std::make_unique<MockMqttClient>();
    }

    void TearDown() override {
        mockMqttClient.reset();
    }

    std::string configFile = "/home/champ/scm/config/schedule.config";
    std::unique_ptr<MockMqttClient> mockMqttClient;
};

TEST_F(MqttControllerIntegrationTest, StartValidConfig) 
{
    config_table_type configTable = {{"key", "value"}};

    EXPECT_CALL(*mockMqttClient, start())
        .WillOnce(testing::Return());

    MqttController mqttController(configTable);
    mqttController.setMqttClient(std::move(mockMqttClient));

    int result = mqttController.start();
    EXPECT_EQ(result, SCM::SUCCESS); 
}

TEST_F(MqttControllerIntegrationTest, StartInvalidConfig) 
{
    config_table_type invalidConfigTable = {{"", ""}};

    MqttController mqttController(configFile);
    mqttController.setMqttClient(std::move(mockMqttClient));

    int result = mqttController.start();
    EXPECT_EQ(result, SCM::FAILED); 
}

#endif