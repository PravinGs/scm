#ifndef IDS_CONTROLLER_INTEGRATION_TEST_HPP
#define IDS_CONTROLLER_INTEGRATION_TEST_HPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "ids/IdsController.hpp"

class MockIdsService : public IdsService {
public:
    MOCK_METHOD(void, start, (config_table_type&), (override));
    MOCK_METHOD(void, rootkitAnalysis, (), (override));
};

class IdsIntegrationTest : public ::testing::Test 
{
    void SetUp() override 
    {
        mockIdsService = std::make_unique<MockIdsService>();
    }

    void TearDown() override 
    {
        mockIdsService.reset();
    }

    std::string configFile = "/home/champ/scm/config/schedule.config";
    std::unique_ptr<MockIdsService> mockIdsService;
};

TEST_F(IdsIntegrationTest, StartValidConfig) 
{
    config_table_type configTable = {{"key", "value"}};

    EXPECT_CALL(*mockIdsService, start(testing::_))
        .WillOnce(testing::Return());
    
    IdsController idsController(configFile);
    idsController.setIdsService(std::move(mockIdsService));
    int result = idsController.start();
    EXPECT_EQ(result, SCM::SUCCESS); 
}

TEST_F(IdsIntegrationTest, StartInvalidConfig) 
{
    config_table_type invalidConfigTable = {{"", ""}};

    IdsController idsController(configFile);
    idsController.setIdsService(std::move(mockIdsService));
    int result = idsController.start();
    EXPECT_EQ(result, SCM::FAILED); 
}

TEST_F(IdsIntegrationTest, StartRootAnalysisValidConfig) 
{
    config_table_type configTable = {{"key", "value"}};

    EXPECT_CALL(*mockIdsService, rootkitAnalysis())
        .WillOnce(testing::Return());
    
    IdsController idsController(configFile);
    idsController.setIdsService(std::move(mockIdsService));
    int result = idsController.startRootAnalysis();
    EXPECT_EQ(result, SCM::SUCCESS); 
}

#endif