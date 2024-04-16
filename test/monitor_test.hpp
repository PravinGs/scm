#ifndef MONITOR_UNIT_TEST_HPP
#define MONITOR_UNIT_TEST_HPP 
#pragma once

#include "monitor/monitor_service_impl.hpp" 
#include <gtest/gtest.h>   
 
struct MONITOR_TEST : public testing::Test
{
    monitor_service_impl  monitor;
    void setup() {   }
    void TearDown() {  }
    };

 
// TEST_F(MONITOR_TEST , GetProcessNameIdWithRunningProcessId)
// {
//     const unsigned int process_id = 200 ;
//     string response = monitor.get_process_name_id(process_id);
//     DEBUG(response.empty() ? "No proces exist with this id: 200" : response);
//     EXPECT_GT(response.length(), 0);
//     // EXPECT_STREQ(response , SUCCESS);
// }
 
// TEST_F(MONITOR_TEST, GetProcessNameIdWithInvalidProcessId)
// {
//    const unsigned int process_id = 1111222;
//    string response = monitor.get_process_name_id(process_id) ;
//    EXPECT_EQ(response.length(), 0);
//    //EXPECT_EQ(response , FAILED);
// }
 
 TEST_F(MONITOR_TEST , ReadProcessTimeIdValidInput)
 {
     const unsigned int process_id = 2328;
     cpu_table response = monitor.read_processing_time_id(process_id) ;
     double cpuTime = monitor.calculate_cpu_time(response);
     EXPECT_GT(cpuTime ,  0);     

 }
 
//    TEST_F(MONITOR_TEST , ReadProcessTimeIdInValidInput) 
//  {
//        const unsigned int process_id = 14 ;
//        cpu_table response = monitor.read_processing_time_id(process_id) ;
//        double cpuTime = monitor.calculate_cpu_time(response);
//        EXPECT_GT(cpuTime, 0);     
// }

// TEST_F(MONITOR_TEST , GetProcessNameIdWithRunningProcessId)
// {
//     const unsigned int process_id = 2328 ;
//     string response = monitor.get_process_name_id(process_id);
//     DEBUG(response.empty() ? "No proces exist with this id: 1505" : response);
//     EXPECT_GT(response.length(), 0);
//     //EXPECT_STREQ(response , SUCCESS);
// }

//  TEST_F(MONITOR_TEST , CalculateCpuTimeValidInput)
// {
//     const unsigned int table =  ;
//     double response = monitor.calculate_cpu_time(cpu_table) ;
//     EXPECT_EQ(response, SUCCESS);
//  }
 
// TEST_F(MONITOR_TEST ,CalculateCpuTimeInValidInput)
// {
//     const unsigned int table = ;
//     double response = monitor.calculate_cpu_time(cpu_table) ;
//     EXPECT_EQ(response ,FAILED);
// }
 
// TEST_F(MONITOR_TEST, GetMemoryUsageValidInput)
// {
//     const unsigned int process_id = 1;
//     double response = monitor.get_memory_usage(process_id);
//     // DEBUG(response.empty() ? "No proces exist with this id: 1" : response);
//     // EXPECT_GT(response.length(), 0);
//     EXPECT_EQ(response , SUCCESS);
// }

 
// TEST_F(MONITOR_TEST , GetMemoryUsageInValidInput)
// {
//     const unsigned int process_id = 1243589;
//     double response = monitor.get_memory_usage(process_id) ;
//     DEBUG(response.empty() ? "No proces exist with this id: 1243589" : response);
//     EXPECT_GT(response.length(), 0);
//     EXPECT_EQ(response ,FAILED);
// }
 // // TEST_F(MONITOR_TEST , GetProcessNameIdWithRunningProcessId)
// // {
// //     const unsigned int process_id = 2328 ;
// //     string response = monitor.get_process_name_id(process_id);
// //     DEBUG(response.empty() ? "No proces exist with this id: 1505" : response);
// //     EXPECT_GT(response.length(), 0);
// //     //EXPECT_STREQ(response , SUCCESS);
// // }
 
//     EXPECT_GT(response.length(), 0);
    
//     EXPECT_EQ(response ,SUCCESS);
// }
 
// TEST_F(MONITOR_TEST ,  GetDiskUsageInValidInput)
// {
//     const unsigned int process_id = 234356;
//     double response = monitor.get_disk_usage(process_id) ;
//     DEBUG(response.empty() ? "No proces exist with this id: 234356" : response);
//     EXPECT_GT(response.length(), 0);
//     EXPECT_EQ(response ,FAILED);
// }
 
// TEST_F(MONITOR_TEST , CreateProcessDataValid)
// {
//     const unsigned int process_id = 681;
//     process_data response = monitor.create_process_data(process_id) ;
//     DEBUG(response.empty() ? "No proces exist with this id: 681" : response);
//     EXPECT_GT(response.length(), 0);
//     EXPECT_EQ(response , SUCCESS);
// }
 
//  TEST_F(MONITOR_TEST , CreateProcessDataWithInValidInput)
// {
//    const unsigned int process_id = 1234567;
//    process_data response = monitor.create_process_data(process_id) ;
//    DEBUG(response.empty() ? "No proces exist with this id: 1234567" : response);
//     EXPECT_GT(response.length(), 0);
//     EXPECT_EQ(response ,FAILED);
//  }
 
 
                // yogan.s\Downloads ";
//     entity.time_pattern = "0 */1 * * * * ";
//     entity.storage_type = "json" ;
 
//     static int response = monitor.get_monitor_data(entity);
   
//     EXPECT_EQ(response, SUCCESS);
 
// }
 
// TEST_F(MONITOR_TEST, TestGetMonitorDataInvalid)
// {
//     process_entity entity;
//     entity.write_path = " C:\Users\yogan.s\Downloads ";
//     entity.time_pattern = "* * * * 0 * /";
//     entity.storage_type = "json" ;
 
//     static int response = monitor.get_monitor_data(entity);
   
//     EXPECT_EQ(response, FAILED);
 
// }
 
// TEST_F(MONITOR_TEST, TestGetMonitorValid)
// {
//     process_entity entity;
//     entity.write_path = " C:\Users\yogan.s\Downloads ";
//     entity.time_pattern = "0 */1 * * * * ";
//     entity.storage_type = "json" ;
//     static int response = monitor.get_monitor_data(entity);
//     EXPECT_EQ(response, SUCCESS);
 
// }
 
// TEST_F(MONITOR_TEST, TestGetMonitorDataInvalidType)
// {
//     process_entity entity;
//     entity.write_path = " C:\Users\yogan.s\Downloads ";
//     entity.time_pattern = "0 */1 * * * * ";
//     entity.storage_type = " " ;
 
//     static int response = monitor.get_monitor_data(entity);
//     EXPECT_EQ(response, FAILED);
 
// }
 
// TEST_F(MONITOR_TEST, TestGetMonitorDataInvalidPath) {
//    process_entity entity;  
//    entity.write_path = "E:\Users\yogan.s\Downloads";
//    entity.time_pattern = "0 */1 * * * *  ";
//    entity.storange_type = "json";
   
//    int response = monitor.get_monitor_data(entity);
//    EXPECT_EQ(response,FAILED);  
//    }
 
// TEST_F(MONITOR_TEST, TestGetMonitorDataInvalidPathPattern)
// {
//    process_entity entity;  
//    entity.write_path = " ";
//    entity.time_pattern = " ";
//    entity.storage_type = "json";
   
//    int response = monitor.get_monitor_data(entity);
//    EXPECT_EQ(response,FAILED);  
// }
 
// TEST_F(MONITOR_TEST, TestGetMonitorDataInvalidPathStorageType) {
//    process_entity entity;  
//    entity.write_path = "C:\Users\yogan.s\Desktop";
//    entity.time_pattern = "0 */1 * * * *  ";
//    entity.storange_type = "ahsh";
   
//    int response = monitor.get_monitor_data(entity);
//    EXPECT_EQ(response,FAILED);  
//    }
 
//    TEST_F(MONITOR_TEST, TestGetMonitorDataInvalidPathStorageType) {
//    process_entity entity;  
//    entit[       OK ] MONITOR_TEST.ReadProcessTimeIdValidInput (0 ms)y.write_path = "F:\Users\yogan.s\Downloads";
//    entity.time_pattern = "0 */1 * * * *  ";
//    entity.storange_type = "ahsh";
   
//    int response = monitor.get_monitor_data(entity);
//    EXPECT_EQ(response,FAILED);  
//    }
 
//    TEST_F(MONITOR_TEST, TestGetMonitorDataPatternType) {
//    process_entity entity;  
//    entity.write_path = "C:\Users\yogan.s\Downloads";
//    entity.time_pattern = "0 * * * */1 *";
//    entity.storange_type = " ";
   
//    int response = monitor.get_monitor_data(entity);
//    EXPECT_EQ(response,FAILED);  
//    }
 
// TEST_F(MONITOR_TEST, TestGetMonitorDataValid) {
   
//    process_entity entity;
//    entity.write_path = "C:\\Users\\yogan.s\\Downloads";
//    entity.time_pattern = "0 */1 *** *";
//    entity.storage_type = "json";
//    std::vector<process_data> parent;
   
//    int response = monitor.get_monitor_data(entity, parent);
 
//    EXPECT_EQ(response, SUCCESS);
// }
 

// TEST_F(MONITOR_TEST, TestGetMonitorDataInValidTimePatternStorageInput) {
   
//    process_entity entity;
//    entity.write_path = "C:\\Users\\yogan.s\\Downloads";
//    entity.time_pattern = "0 */1 *** *";
//    entity.storage_type = "cfcf";
//    std::vector<process_data> parent;
   
//    int response = monitor.get_monitor_data(entity, parent);
 
//    EXPECT_EQ(response, FAILED);
// }
 
// TEST_F(MONITOR_TEST, TestGetMonitorDataInValidWriteStorageInput) {
   
//    process_entity entity;
//    entity.write_path = "H:\\Users\\yogan.s\\Downloads";
//    entity.time_pattern = "0 */1 *** *";
//    entity.storage_type = "cfcf";
//    std::vector<process_data> paTEST_F(MONITOR_TEST, TestGetMonitorDataValid) {
   
//    process_entity entity;
//    entity.write_path = "C:\\Users\\yogan.s\\Downloads";
//    entity.time_pattern = "0 */1 *** *";
//    entity.storage_type = "cfcf";
//    std::vector<process_data> parent;
   
//    int response = monitor.get_monitor_data(entity, parent);
 
//    EXPECT_EQ(response, FAILED);
// }
//  rent;
   
//    int response = monitor.get_monitor_data(entity, parent);
 
//    EXPECT_EQ(response, FAILED);
// }
 
// TEST_F(MONITOR_TEST, TestGetMonitorDataInValidWriteTIMEInput) {
   
//    process_entity entity;
//    entity.write_path = "H:\\Users\\yogan.s\\Downloads";
//    entity.time_pattern = "0 */1 ***3";
//    entity.storage_type = "cfcf";
//    std::vector<process_data> parent;
   
//    int response = monitor.get_monitor_data(entity, parent);
 
//    EXPECT_EQ(response, FAILED);
// }

// TEST_F(MONITOR_TEST, TestGetMonitorDataValid) {
   
//    process_entity entity;
//    entity.write_path = "C:\\Users\\yogan.s\\Downloads";
//    entity.time_pattern = "0 */1 *** *";
//    entity.storage_type = "cfcf";
//    std::vector<process_data> parent;
   
//    int response = monitor.get_monitor_data(entity, parent);
 
//    EXPECT_EQ(response, FAILED);
// }
 //#endif




//  TEST_F(MONITOR_TEST, GetMemoryUsageValidInput)
// {
//     const unsigned int process_id = 113;
//     double response = monitor.get_memory_usage(process_id) ;
//     EXPECT_EQ(response ,SUCCESS); 

#endif