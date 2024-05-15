// #ifndef MQTT_SERVICE_UNIT_TEST_HPP
// #define MQTT_SERVICE_UNIT_TEST_HPP
// #pragma once 
// #include "util/Common.hpp"
// #include "mqtt/mqtt_service.hpp"
// #include "util/ConfigService.hpp"
// #include "util/entity_parser.hpp"
// #include <gtest/gtest.h>

// struct mqtt_test : public testing::Test
// {
//     const std::string config_file_path = "/home/champ/scm/ConfigService/schedule.ConfigService";
   
//     std::unique_ptr<MqttClient> MqttClient_instance = nullptr;

//     void SetUp()
//     {
//         config_table_type table;
//         ConfigService configService;
//         entity_parser parser;

//         int result = configService.readIniConfigFile(config_file_path, table);
//         if (result != SCM::SUCCESS)
//         {
//             return;
//         }

//         MqttEntity entity = parser.getMqttEntity(table);
//         MqttClient_instance = std::make_unique<MqttClient>(entity, table);
//     }

//     void TearDown(){}
// };

// TEST_F(mqtt_test, sample_test)
// {
//     if (MqttClient_instance){
//         std::cout << "Instance created\n";
//     }


// }

// #endif