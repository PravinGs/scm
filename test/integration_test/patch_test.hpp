#ifndef PATCH_CONTROLLER_INTEGRATION_TEST_HPP
#define PATCH_CONTROLLER_INTEGRATION_TEST_HPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "patch/PatchController.hpp"

class MockPatchProxy : public PatchProxy {
public:
    MOCK_METHOD(void, start, (const config_table_type&), (override));
};

class PatchControllerTest : public ::testing::Test {
    void SetUp() override {
        mockPatchProxy = std::make_unique<MockPatchProxy>();
    }

    void TearDown() override {
        mockPatchProxy.reset();
    }

    std::string configFile = "/home/champ/scm/config/schedule.config";

    std::unique_ptr<MockPatchProxy> mockPatchProxy;
};

TEST_F(PatchControllerTest, StartValidConfig) {
    config_table_type configTable = {{"key", "value"}};

    EXPECT_CALL(*mockPatchProxy, start(testing::_))
        .WillOnce(testing::Return());

    PatchController patchController(configFile);
    patchController.setPatchService(std::move(mockPatchProxy));
    int result = patchController.start();
    EXPECT_EQ(result, SCM::SUCCESS); 

}

TEST_F(PatchControllerTest, StartInvalidConfig) 
{
    config_table_type invalidConfigTable = {{"", ""}};

    PatchController patchController(configFile);
    patchController.setPatchService(std::move(mockPatchProxy));
    int result = patchController.start();
    EXPECT_EQ(result, SCM::FAILED); 

}

#endif