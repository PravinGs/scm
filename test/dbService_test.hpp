#include "util/Common.hpp"
#include "db/DbService.hpp"
using namespace SCM::Tpm;

struct DB_TEST : public testing::Test
{
    DbService dbService = TPM_CONTEXT_DB_PATH;

    void SetUp()
    {
    }
    void TearDown()
    {
    }
};

TEST_F(DB_TEST, AddTpmContextWithValidInput)
{
    TpmContext handle;
    handle.keyName = "def";
    handle.dekAuth = "Password";
    int response = dbService.addTpmContext(handle);
    EXPECT_EQ(response, TPM2_SUCCESS);
    int del_result = dbService.deleteHandle(handle);
    EXPECT_EQ(del_result, TPM2_SUCCESS);
}

TEST_F(DB_TEST, AddTpmContextWithEmptyKeyName)
{
    TpmContext handle;
    handle.keyName = "";
    handle.dekAuth = "dekAuth";
    handle.index;
    handle.dataSize;
    int response = dbService.addTpmContext(handle);
    EXPECT_EQ(response, TPM2_INVALID_CONTEXT);
}

TEST_F(DB_TEST, AddTpmContextWithEmptyPassword)
{
    TpmContext handle;
    handle.keyName = "abc";
    handle.dekAuth = "";
    handle.index;
    handle.dataSize;
    int response = dbService.addTpmContext(handle);
    EXPECT_EQ(response, TPM2_INVALID_CONTEXT);
}

TEST_F(DB_TEST, GetTpmContextByKeyNameAndDekAuthWithValidInput)
{
    TpmContext handle;
    handle.keyName = "abc";
    handle.dekAuth = "dekAuth";
    handle.index = 125436;
    int response = dbService.addTpmContext(handle);
    EXPECT_EQ(response, TPM2_SUCCESS);
    int result = dbService.getTpmContextByKeyNameAndDekAuth(handle);
    EXPECT_EQ(handle.index, 125436);
    int del_result = dbService.deleteHandle(handle);
    EXPECT_EQ(del_result, TPM2_SUCCESS);
}

TEST_F(DB_TEST, GetTpmContextByKeyNameAndDekAuthWithInValidKeyName)
{
    TpmContext handle;
    string keyName = "";
    string dekAuth = "dekAuth";
    int result = dbService.getTpmContextByKeyNameAndDekAuth(handle);
    EXPECT_NE(handle.index, 125436);
}

TEST_F(DB_TEST, GetTpmContextByKeyNameAndDekAuthWithInValidDekAuth)
{
    TpmContext handle;
    string keyName = "abc";
    string dekAuth = "";
    int result = dbService.getTpmContextByKeyNameAndDekAuth(handle);
    EXPECT_NE(handle.index, 125436);
}

TEST_F(DB_TEST, GetTpmContextByIndexWithValidInput)
{
    TpmContext handle;
    handle.keyName = "abc";
    handle.dekAuth = "dekAuth";
    handle.index = 0;
    int response = dbService.addTpmContext(handle);
    EXPECT_EQ(response, TPM2_SUCCESS);
    int result = dbService.getTpmContextByIndex(handle.index);
    EXPECT_EQ(result, TPM2_DUPLICATE_VALUE);
    int del_result = dbService.deleteHandle(handle);
    EXPECT_EQ(del_result, TPM2_SUCCESS);
}

TEST_F(DB_TEST, GetTpmContextByIndexWithInValidInput)
{
    TpmContext handle;
    handle.index = 125436;
    int result = dbService.getTpmContextByIndex(handle.index);
    EXPECT_NE(result, TPM2_DUPLICATE_VALUE);
}

TEST_F(DB_TEST, DeleteHandleWithValidInput)
{
    TpmContext handle;
    handle.keyName = "abc";
    handle.dekAuth = "dekAuth";
    int response = dbService.addTpmContext(handle);
    EXPECT_EQ(response, TPM2_SUCCESS);
    int result = dbService.deleteHandle(handle);
    EXPECT_EQ(result, TPM2_SUCCESS);
}

TEST_F(DB_TEST, DeleteHandleWithInvalidPassword)
{
    TpmContext handle;
    handle.keyName = "abc";
    handle.dekAuth = "";
    int result = dbService.deleteHandle(handle);
    EXPECT_NE(result, TPM2_SUCCESS);
}

TEST_F(DB_TEST, GetTpmContextByNameWithValidInput)
{
    TpmContext handle;
    handle.keyName = "abc";
    handle.dekAuth = "dekAuth";
    int response = dbService.addTpmContext(handle);
    EXPECT_EQ(response, TPM2_SUCCESS);
    int result = dbService.getTpmContextByName(handle.keyName);
    EXPECT_EQ(result, TPM2_DUPLICATE_VALUE);
    int del_result = dbService.deleteHandle(handle);
    EXPECT_EQ(del_result, TPM2_SUCCESS);
}

TEST_F(DB_TEST, GetTpmContextByNameWithInValidInput)
{
    TpmContext handle;
    handle.keyName = "";
    int result = dbService.getTpmContextByName(handle.keyName);
    EXPECT_NE(result, TPM2_DUPLICATE_VALUE);
}

TEST_F(DB_TEST, DeleteTpmContextAllHandles)
{
    int response = dbService.deleteTpmContextAllHandles();
    EXPECT_EQ(response, TPM2_SUCCESS);
}