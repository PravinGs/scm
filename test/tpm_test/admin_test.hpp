#include <gtest/gtest.h>
#include "util/Common.hpp"
#include "util/libs.hpp"
#include "tpm/TpmAdmin.hpp"
#include <tss2/tss2_sys.h>
using namespace SCM::Tpm;

struct ADMIN_TEST : public testing::Test
{
    TpmAdmin tpmAdmin;
    ESYS_CONTEXT *g_esys_context = nullptr;
    TSS2_TCTI_CONTEXT *g_tcti_context = nullptr;
    bool g_context_state = true;
    const char *lockout_auth = "";
    void SetUp()
    {
        size_t device_size = 0;
        const string DEVICE_ADDRESS = "/dev/tpm0";
        // TPM2_RC  response = Tss2_TctiLdr_Initialize(SIMULATOR_ADDRESS, &g_tcti_context);
        TPM2_RC response = Tss2_Tcti_Device_Init(g_tcti_context, &device_size, DEVICE_ADDRESS.c_str());
        if (response != SCM::Tpm::TPM2_SUCCESS)
        {
            g_context_state = false;
        }
        response = Esys_Initialize(&g_esys_context, g_tcti_context, NULL);
        if (response != SCM::Tpm::TPM2_SUCCESS)
        {
            g_context_state = false;
        }
        // g_context_state = (initialize_db(DEFAULT_DB_PATH) == TPM2_INVALID_DATABASE) ? true : false;
    }
    void TearDown()
    {
        cleanup();
    }

    void cleanup()
    {
        Tss2_Tcti_Finalize(g_tcti_context);
        Esys_Free(g_esys_context);
        g_tcti_context = NULL;
        g_esys_context = NULL;
    }
};

TEST_F(ADMIN_TEST, ChangeTpmOwnerHierarchyPasswordWithValidInput)
{
    string oldAuth = "";
    string newAuth = "owner";
    TPM2_RC response = tpmAdmin.setAuthHierarchy(g_esys_context, ESYS_TR_RH_OWNER, oldAuth, newAuth);
    EXPECT_EQ(response, TPM2_SUCCESS);
}

TEST_F(ADMIN_TEST, ChangeTpmOwnerHierarchyPasswordWithValidInput1)
{
    string oldAuth = "owner";
    string newAuth = "";
    TPM2_RC response = tpmAdmin.setAuthHierarchy(g_esys_context, ESYS_TR_RH_OWNER, "oldAuth", newAuth);
    EXPECT_EQ(response, TPM2_INVALID_HIERARCHY_AUTH);
    response = tpmAdmin.setAuthHierarchy(g_esys_context, ESYS_TR_RH_OWNER, oldAuth, newAuth);
    EXPECT_EQ(response, TPM2_SUCCESS);
}

TEST_F(ADMIN_TEST, ChangeTpmEndorsementHierarchyPassword)
{
    string oldAuth = "";
    string newAuth = "endorsement";
    TPM2_RC response = tpmAdmin.setAuthHierarchy(g_esys_context, ESYS_TR_RH_ENDORSEMENT, oldAuth, newAuth);
    EXPECT_EQ(response, TPM2_SUCCESS);
}

TEST_F(ADMIN_TEST, ChangeTpmEndorsementHierarchyPassword1)
{
    string oldAuth = "endorsement";
    string newAuth = "";
    TPM2_RC response = tpmAdmin.setAuthHierarchy(g_esys_context, ESYS_TR_RH_ENDORSEMENT, "oldAuth", newAuth);
    EXPECT_EQ(response, TPM2_INVALID_HIERARCHY_AUTH);
    response = tpmAdmin.setAuthHierarchy(g_esys_context, ESYS_TR_RH_ENDORSEMENT, oldAuth, newAuth);
    EXPECT_EQ(response, TPM2_SUCCESS);
}

/*TEST_F(ADMIN_TEST, CHANGE_LOCKOUT_HIERARCHY_PASSWORD)
{
    string oldAuth = "";
    string newAuth = "lockout";
    TPM2_RC response = tpmAdmin.setAuthHierarchy(g_esys_context, ESYS_TR_RH_ENDORSEMENT, oldAuth, newAuth);
    EXPECT_EQ(response, TPM2_SUCCESS);
}

TEST_F(ADMIN_TEST, CHANGE_LOCKOUT_HIERARCHY_PASSWORD_2)
{
    string oldAuth = "lockout";
    string newAuth = "";
    TPM2_RC response = tpmAdmin.setAuthHierarchy(g_esys_context, ESYS_TR_RH_ENDORSEMENT, "oldAuth", newAuth);
    EXPECT_EQ(response, TPM2_INVALID_HIERARCHY_AUTH);
    response = tpmAdmin.setAuthHierarchy(g_esys_context, ESYS_TR_RH_ENDORSEMENT, oldAuth, newAuth);
    EXPECT_EQ(response, TPM2_SUCCESS);
}
*/
TEST_F(ADMIN_TEST, CLEAR_TPM_WITH_INVALID_ESYS_CONTEXT)
{
    TPM2_RC response = TPM2_SUCCESS;
    response = tpmAdmin.clearTpm(g_esys_context, lockout_auth);
    ASSERT_TRUE(response == TPM2_SUCCESS);
}

TEST_F(ADMIN_TEST, CLEAR_TPM_WITH_CORRECT_LOCKOUT_PASSWORD)
{
    TPM2_RC response = TPM2_SUCCESS;
    response = tpmAdmin.clearTpm(g_esys_context, lockout_auth);
    std::cout << response;
    ASSERT_TRUE(response == TPM2_SUCCESS);
}
/*
// clear tpm keys and sealed data
TEST_F(ADMIN_TEST, CLEAR_TPM_WITH_CORRECT_LOCKOUT_PASSWORD)
{
    TPM2_RC response = TPM2_SUCCESS;
    // response = setDictionaryLock(g_esys_context);
    resetTpm();
    ASSERT_TRUE(response == TPM2_SUCCESS);
} */