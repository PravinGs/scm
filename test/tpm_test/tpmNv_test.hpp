#include "util/Common.hpp"
#include "tpm/TpmAdmin.hpp"
#include "tpm/TpmNv.hpp"
using namespace SCM::Tpm;

struct NV_READ_WRITE_TEST : public testing::Test
{
    TpmNv tpmNv;
    TpmAdmin tpmAdmin;
    ESYS_CONTEXT *g_esys_context = NULL;
    TSS2_TCTI_CONTEXT *g_tcti_context = NULL;
    bool g_context_state = true;
    const std::string ownerAuth = "";
    const std::string seal_key_path = "/home/champ/Desktop/TPM/x_gtest_tpm/tmp/hash";
    const std::string invalid_seal_key_path = "/home/champ/Downloads/Serial_communication/TPM/x_gtest_tpm/tmp/priva";
    const std::string private_key_1024 = "/home/champ/Desktop/TPM/x_gtest_tpm/tmp/1024_private_key.pem";
    const std::string private_key_2048 = "/home/champ/Desktop/TPM/x_gtest_tpm/tmp/2048_private_key.pem";
    size_t device_size = 0;
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
        // g_context_state = (initialize_db(TPM_CONTEXT_DB_PATH) == TPM2_INVALID_DATABASE) ? true : false;
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

static string readFileToString(const string &path)
{
    FILE *file = NULL;

    uint8_t key_buffer[2048];
    size_t key_buffer_size;
    file = fopen(path.c_str(), "rb");
    if (file == NULL)
    {
        return "";
    }
    else
    {
        key_buffer_size = fread(key_buffer, sizeof(uint8_t), sizeof(key_buffer), file);
        fclose(file);
    }

    return string(reinterpret_cast<char *>(key_buffer), key_buffer_size);
}

// /* seal data with key size is 0*/
TEST_F(NV_READ_WRITE_TEST, StoreDataWithNoKeyData)
{
    SCOPED_TRACE("Seal data into the TPM with the empty key file, expecting this function will return TPM2_EMPTY_KEY_FILE");
    const std::string no_size_key_path = "/home/champ/Desktop/TPM/x_gtest_tpm/tmp/emptyfile";
    TpmContext context;
    context.ownerAuth = "";
    context.keyName = "private-key";
    context.dekAuth = "nvAuth";
    context.data = readFileToString(no_size_key_path);
    context.dataSize = context.data.size();
    TPM2_RC result = tpmNv.nvProxy(g_esys_context, context); // seal data into the tpm.
    EXPECT_EQ(result, TPM2_EMPTY_KEY_FILE);
    result = tpmAdmin.clearTpm(g_esys_context, "");
    ASSERT_EQ(result, TPM2_SUCCESS);
}

// /*DELETE key*/
TEST_F(NV_READ_WRITE_TEST, DeleteKeyWithValidInput)
{
    TPM2_RC response = 0;
    TpmContext context;
    string data = readFileToString(private_key_1024);
    context.data = data;
    context.ownerAuth = "";
    context.keyName = "private-key";
    context.dekAuth = "nvAuth";
    context.dataSize = data.size();
    response = tpmNv.nvProxy(g_esys_context, context);
    EXPECT_EQ(response, TPM2_SUCCESS);
    response = tpmNv.nvDelete(g_esys_context, context);
    EXPECT_EQ(response, TPM2_DUPLICATE_VALUE);
    response = tpmAdmin.clearTpm(g_esys_context, "");
    EXPECT_EQ(response, TPM2_SUCCESS);
}

/*delete key with invalid input*/
TEST_F(NV_READ_WRITE_TEST, DeleteKeyWithInvalidInput)
{
    TPM2_RC response = 0;
    TpmContext context;
    string data = readFileToString(private_key_1024);
    context.data = data;
    context.ownerAuth = "";
    context.keyName = "private-key";
    context.dekAuth = "nvAuth";
    context.dataSize = data.size();
    std::cout << "DataSize: " << context.dataSize;
    response = tpmNv.nvProxy(g_esys_context, context);
    EXPECT_EQ(response, TPM2_SUCCESS);
    context.dekAuth = "nuth";
    response = tpmNv.nvDelete(g_esys_context, context);
    EXPECT_NE(response, TPM2_DUPLICATE_VALUE);
    response = tpmAdmin.clearTpm(g_esys_context, "");
    EXPECT_EQ(response, TPM2_SUCCESS);
}

TEST_F(NV_READ_WRITE_TEST, ReadKeyWithValidInput)
{
    TPM2_RC response = 0;
    TpmContext context;
    context.ownerAuth = "";
    context.keyName = "private-key";
    context.dekAuth = "nvAuth";
    context.data = readFileToString(private_key_1024);
    context.dataSize = context.data.size();
    response = tpmNv.nvProxy(g_esys_context, context);
    EXPECT_EQ(response, TPM2_SUCCESS);
    response = tpmNv.nvRead(g_esys_context, context);
    EXPECT_EQ(response, TPM2_DUPLICATE_VALUE);
    response = tpmAdmin.clearTpm(g_esys_context, "");
    EXPECT_EQ(response, TPM2_SUCCESS);
}

TEST_F(NV_READ_WRITE_TEST, ReadKeyWithInvalidInput)
{
    TPM2_RC response = 0;
    TpmContext context;
    context.ownerAuth = "";
    context.keyName = "private-key";
    context.dekAuth = "nvAuth";
    context.data = readFileToString(private_key_1024);
    context.dataSize = context.data.size();
    response = tpmNv.nvProxy(g_esys_context, context);
    EXPECT_EQ(response, TPM2_SUCCESS);
    context.keyName = "invalid-private-key";
    response = tpmNv.nvRead(g_esys_context, context);
    EXPECT_NE(response, TPM2_DUPLICATE_VALUE);
    response = tpmAdmin.clearTpm(g_esys_context, "");
    EXPECT_EQ(response, TPM2_SUCCESS);
}

/*Store key size is 2048*/
TEST_F(NV_READ_WRITE_TEST, Store2048SizeKey)
{
    TPM2_RC response = 0;
    TpmContext context;
    context.ownerAuth = "";
    context.keyName = "private-key";
    context.dekAuth = "nvAuth";
    context.data = readFileToString("/home/champ/Desktop/TPM/x_gtest_tpm/tmp/2048_private_key.pem");
    context.dataSize = context.data.size();
    response = tpmNv.nvProxy(g_esys_context, context);
    EXPECT_EQ(response, TPM2_EXCEEDED_MAX_NV_BUFFER_SIZE);
    response = tpmAdmin.clearTpm(g_esys_context, "");
    EXPECT_EQ(response, TPM2_SUCCESS);
}

/*Store key size is 1024*/
TEST_F(NV_READ_WRITE_TEST, Store1024SizeKey)
{
    TPM2_RC response = 0;
    TpmContext context;
    context.ownerAuth = "";
    context.keyName = "private-key-2";
    context.dekAuth = "nvAuth";
    context.data = readFileToString(private_key_1024);
    context.dataSize = context.data.size();
    response = tpmNv.nvProxy(g_esys_context, context);
    EXPECT_EQ(response, TPM2_SUCCESS);
    response = tpmAdmin.clearTpm(g_esys_context, "");
    EXPECT_EQ(response, TPM2_SUCCESS);
}

TEST_F(NV_READ_WRITE_TEST, DeleteKeyWithInvalidEsysContext)
{
    ESYS_CONTEXT *esys_context = nullptr;
    TPM2_RC response = 0;
    TpmContext context;
    context.ownerAuth = "";
    context.keyName = "private-key";
    context.dekAuth = "nvAuth";
    context.data = readFileToString(private_key_1024);
    context.dataSize = context.data.size();
    response = tpmNv.nvProxy(esys_context, context);
    EXPECT_EQ(response, TPM2_INVALID_ESYS_CONTEXT);
    context.dekAuth = "nuth";
    response = tpmNv.nvDelete(esys_context, context);
    EXPECT_EQ(response, TPM2_INVALID_ESYS_CONTEXT);
    response = tpmAdmin.clearTpm(esys_context, "");
    EXPECT_EQ(response, TPM2_INVALID_ESYS_CONTEXT);
}