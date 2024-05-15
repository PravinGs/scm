#ifndef MONITOR_UNIT_TEST_HPP
#define MONITOR_UNIT_TEST_HPP
#pragma once

#include "monitor/MonitorServiceImpl.hpp"
#include <gtest/gtest.h>

struct MONITOR_TEST : public testing::Test
{
    MonitorServiceImpl monitor;
    void setup() {}
    void TearDown() {}
};

// TEST_F(MONITOR_TEST , GetProcessNameIdWithRunningProcessId)
// {
//     const unsigned int process_id = 200 ;
//     string response = monitor.getProcessNameById(process_id);
//     DEBUG(response.empty() ? "No proces exist with this id: 200" : response);
//     EXPECT_GT(response.length(), 0);
//     // EXPECT_STREQ(response , SUCCESS);
// }

// TEST_F(MONITOR_TEST, GetProcessNameIdWithInvalidProcessId)
// {
//    const unsigned int process_id = 1111222;
//    string response = monitor.getProcessNameById(process_id) ;
//    EXPECT_EQ(response.length(), 0);
//    //EXPECT_EQ(response , FAILED);
// }

// TEST_F(MONITOR_TEST, ReadProcessTimeIdValidInput) //
// {
//     const unsigned int process_id = 2328;
//     CpuTable TABLE = monitor.getProcessTimeById(process_id);
//     double cpuTime = monitor.calculateCpuTime(TABLE);
//     EXPECT_GT(cpuTime, 0);    
// }

//    TEST_F(MONITOR_TEST , ReadProcessTimeIdInValidInput)
//  {
//        const unsigned int process_id = 14 ;
//        CpuTable response = monitor.getProcessTimeById(process_id) ;
//        double cpuTime = monitor.calculateCpuTime(response);
//        EXPECT_GT(cpuTime, 0);
// }

// TEST_F(MONITOR_TEST , GetProcessNameIdWithRunningProcessId)
// {
//     const unsigned int process_id = 2328 ;
//     string response = monitor.getProcessNameById(process_id);
//     DEBUG(response.empty() ? "No proces exist with this id: 1505" : response);
//     EXPECT_GT(response.length(), 0);
//     //EXPECT_STREQ(response , SUCCESS);
// }

//  TEST_F(MONITOR_TEST , CalculateCpuTimeValidInput)
// {
//     const unsigned int table =  ;
//     double response = monitor.calculateCpuTime(CpuTable) ;
//     EXPECT_EQ(response, SUCCESS);
//  }

// TEST_F(MONITOR_TEST ,CalculateCpuTimeInValidInput)
// {
//     const unsigned int table = ;
//     double response = monitor.calculateCpuTime(CpuTable) ;
//     EXPECT_EQ(response ,FAILED);
// }

// TEST_F(MONITOR_TEST, GetMemoryUsageValidInput)
// {
//     const unsigned int process_id = 1;
//     double response = monitor.getMemoryUsage(process_id);
//     // DEBUG(response.empty() ? "No proces exist with this id: 1" : response);
//     // EXPECT_GT(response.length(), 0);
//     EXPECT_EQ(response , SUCCESS);
// }

// TEST_F(MONITOR_TEST , GetMemoryUsageInValidInput)
// {
//     const unsigned int process_id = 1243589;
//     double response = monitor.getMemoryUsage(process_id) ;
//     DEBUG(response.empty() ? "No proces exist with this id: 1243589" : response);
//     EXPECT_GT(response.length(), 0);
//     EXPECT_EQ(response ,FAILED);
// }
// // TEST_F(MONITOR_TEST , GetProcessNameIdWithRunningProcessId)
// // {
// //     const unsigned int process_id = 2328 ;
// //     string response = monitor.getProcessNameById(process_id);
// //     DEBUG(response.empty() ? "No proces exist with this id: 1505" : response);
// //     EXPECT_GT(response.length(), 0);
// //     //EXPECT_STREQ(response , SUCCESS);
// // }

//     EXPECT_GT(response.length(), 0);

//     EXPECT_EQ(response ,SUCCESS);
// }

// TEST_F(MONITOR_TEST ,  GetDiskUsageInValidInput)
//
//     const unsigned int process_id = 234356;
//     double response = monitor.getDiskUsage(process_id) ;
//     DEBUG(response.empty() ? "No proces exist with this id: 234356" : response);
//     EXPECT_GT(response.length(), 0);
//     EXPECT_EQ(response ,FAILED);
// }

// TEST_F(MONITOR_TEST , CreateProcessDataValid)
// {
//     const unsigned int process_id = 681;
//     ProcessData response = monitor.createProcessData(process_id) ;
//     DEBUG(response.empty() ? "No proces exist with this id: 681" : response);
//     EXPECT_GT(response.length(), 0);
//     EXPECT_EQ(response , SUCCESS);
// }

//  TEST_F(MONITOR_TEST , CreateProcessDataWithInValidInput)
// {
//    const unsigned int process_id = 1234567;
//    ProcessData response = monitor.createProcessData(process_id) ;
//    DEBUG(response.empty() ? "No proces exist with this id: 1234567" : response);
//     EXPECT_GT(response.length(), 0);
//     EXPECT_EQ(response ,FAILED);
//  }



// TEST_F(MONITOR_TEST, TestGetMonitorDataValid)
// {
//     ProcessEntity entity;
//     entity.write_path = "  ";
//     entity.time_pattern = "0 */1 * * * *";
//     entity.storage_type = "json";
//     vector<ProcessData> logs;
//     static int response = monitor.getAppResourceDetails(entity, logs);
//     EXPECT_EQ(response, SUCCESS);
// }

TEST_F(MONITOR_TEST, TestGetMonitorDataInvalidType)
{
    ProcessEntity entity;
    entity.write_path = "";
    entity.time_pattern = ""; 
    entity.storage_type = "json" ; 
    vector<ProcessData> logs;
    static int response = monitor.getAppResourceDetails(entity , logs);
    EXPECT_EQ(response, SCM::FAILED);
}

// TEST_F(MONITOR_TEST, TestGetMonitorDataInvalidPath) {
//    ProcessEntity entity;
//    entity.write_path = "E:\Users\yogan.s\Downloads";
//    entity.time_pattern = "0 */1 * * * *  ";
//    entity.storange_type = "json";

//    int response = monitor.get_monitor_data(entity);
//    EXPECT_EQ(response,FAILED);
//    }

// TEST_F(MONITOR_TEST, TestGetMonitorDataInvalidPathPattern)
// {
//    ProcessEntity entity;
//    entity.write_path = " ";
//    entity.time_pattern = " ";
//    entity.storage_type = "json";

//    int response = monitor.get_monitor_data(entity);
//    EXPECT_EQ(response,FAILED);
// }

// TEST_F(MONITOR_TEST, TestGetMonitorDataInvalidPathStorageType) {
//    ProcessEntity entity;
//    entity.write_path = "C:\Users\yogan.s\Desktop";
//    entity.time_pattern = "0 */1 * * * *  ";
//    entity.storage_type = "ahsh";

//    int response = monitor.get_monitor_data(entity);
//    EXPECT_EQ(response,FAILED);
//    }

//    TEST_F(MONITOR_TEST, TestGetMonitorDataInvalidPathStorageType) {
//    ProcessEntity entity;
//     MONITOR_TEST.ReadProcessTimeIdValidInp.write_path = "F:\Users\yogan.s\Downloads";
//    entity.time_pattern = "0 */1 * * * *  ";
//    entity.storange_type = "ahsh";

//    int response = monitor.get_monitor_data(entity);
//    EXPECT_EQ(response,FAILED);
//    }

//    TEST_F(MONITOR_TEST, TestGetMonitorDataPatternType) {
//    ProcessEntity entity;
//    entity.write_path = "C:\Users\yogan.s\Downloads";
//    entity.time_pattern = "0 * * * */1 *";
//    entity.storange_type = " ";

//    int response = monitor.get_monitor_data(entity);
//    EXPECT_EQ(response,FAILED);
//    }

// TEST_F(MONITOR_TEST, TestGetMonitorDataValid) {

//    ProcessEntity entity;
//    entity.write_path = "C:\\Users\\yogan.s\\Downloads";
//    entity.time_pattern = "0 */1 *** *";
//    entity.storage_type = "json";
//    std::vector<ProcessData> parent;

//    int response = monitor.get_monitor_data(entity, parent);

//    EXPECT_EQ(response, SUCCESS);
// }

// TEST_F(MONITOR_TEST, TestGetMonitorDataInValidTimePatternStorageInput) {

//    ProcessEntity entity;
//    entity.write_path = "C:\\Users\\yogan.s\\Downloads";
//    entity.time_pattern = "0 */1 *** *";
//    entity.storage_type = "cfcf";
//    std::vector<ProcessData> parent;

//    int response = monitor.get_monitor_data(entity, parent);

//    EXPECT_EQ(response, FAILED);
// }

// TEST_F(MONITOR_TEST, TestGetMonitorDataInValidWriteStorageInput) {

//    ProcessEntity entity;
//    entity.write_path = "H:\\Users\\yogan.s\\Downloads";
//    entity.time_pattern = "0 */1 *** *";
//    entity.storage_type = "cfcf";
//    std::vector<ProcessData> paTEST_F(MONITOR_TEST, TestGetMonitorDataValid) {

//    ProcessEntity entity;
//    entity.write_path = "C:\\Users\\yogan.s\\Downloads";
//    entity.time_pattern = "0 */1 *** *";
//    entity.storage_type = "cfcf";
//    std::vector<ProcessData> parent;

//    int response = monitor.get_monitor_data(entity, parent);

//    EXPECT_EQ(response, FAILED);
// }
//  rent;

//    int response = monitor.get_monitor_data(entity, parent);

//    EXPECT_EQ(response, FAILED);
// }

// TEST_F(MONITOR_TEST, TestGetMonitorDataInValidWriteTIMEInput) {

//    ProcessEntity entity;
//    entity.write_path = "H:\\Users\\yogan.s\\Downloads";
//    entity.time_pattern = "0 */1 ***3";
//    entity.storage_type = "cfcf";
//    std::vector<ProcessData> parent;

//    int response = monitor.get_monitor_data(entity, parent);

//    EXPECT_EQ(response, FAILED);
// }

// TEST_F(MONITOR_TEST, TestGetMonitorDataValid) {

//    ProcessEntity entity;
//    entity.write_path = "C:\\Users\\yogan.s\\Downloads";
//    entity.time_pattern = "0 */1 *** *";
//    entity.storage_type = "cfcf";
//    std::vector<ProcessData> parent;

//    int response = monitor.get_monitor_data(entity, parent);

//    EXPECT_EQ(response, FAILED);
// }


//  TEST_F(MONITOR_TEST, GetMemoryUsageValidInput)
// {
//     const unsigned int process_id = 113;
//     double response = monitor.getMemoryUsage(process_id) ;
//     EXPECT_EQ(response ,SUCCESS);

#endif