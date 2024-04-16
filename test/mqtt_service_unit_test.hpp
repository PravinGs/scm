// #ifndef MQTT_SERVICE_UNIT_TEST_HPP
// #define MQTT_SERVICE_UNIT_TEST_HPP
// #pragma once 
// #include "util/common.hpp"
// #include "mqtt/mqtt_service.hpp"
// #include "util/config.hpp"
// #include "util/entity_parser.hpp"
// #include <gtest/gtest.h>

// struct mqtt_test : public testing::Test
// {
//     const std::string config_file_path = "/home/champ/scm/config/schedule.config";
   
//     std::unique_ptr<mqtt_client> mqtt_client_instance = nullptr;

//     void SetUp()
//     {
//         config_table_type table;
//         config config_s;
//         entity_parser parser;

//         int result = config_s.read_ini_config_file(config_file_path, table);
//         if (result != Audit::SUCCESS)
//         {
//             return;
//         }

//         mqtt_entity entity = parser.get_mqtt_entity(table);
//         mqtt_client_instance = std::make_unique<mqtt_client>(entity, table);
//     }

//     void TearDown(){}
// };

// TEST_F(mqtt_test, sample_test)
// {
//     if (mqtt_client_instance){
//         std::cout << "Instance created\n";
//     }


// }

// #endif