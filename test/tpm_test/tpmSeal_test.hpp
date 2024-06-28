#include "util/Common.hpp"
#include "tpm/TpmEntity.hpp"
#include "tpm/TpmAdmin.hpp"
#include "tpm/TpmSealHash.hpp"

using namespace SCM::Tpm;

struct SEAL_TEST : public testing::Test
{
    TpmSeal tpmSeal;
    DbService dbService = TPM_CONTEXT_DB_PATH;
    TpmAdmin tpmAdmin;
    ESYS_CONTEXT *g_esys_context = nullptr;
    TSS2_TCTI_CONTEXT *g_tcti_context = nullptr;
    size_t device_size;
    bool g_context_state = true;
    const std::string ownerAuth = "";
    const std::string seal_key_path = "/home/champ/Desktop/TPM/x_gtest_tpm/tmp/hash";
    const std::string invalid_seal_key_path = "/home/champ/Downloads/Serial_communication/TPM/x_gtest_tpm/tmp/priva";
    
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

string readFile(const string &path)
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

// /*delete existng sealed object*/
// TEST_F(SEAL_TEST, DeleteSealDataWithValidInput)
// {
//     if (!g_context_state)
//     {
//         if (db(TPM_CONTEXT_DB_PATH) == TPM2_INVALID_DATABASE)
//         {
//             GTEST_SKIP() << "Skipping to next test\n";
//         }
//     }
//     TpmContext context;
//     context.ownerAuth = "";
//     context.srkAuth = "srkAuth";
//     context.dekAuth = "dekAuth";
//     context.fileName = seal_key_path;
//     context.keyName = "seal-key";
//     TPM2_RC result = tpmSeal.add(g_esys_context, context); // seal data into the tpm.
//     EXPECT_EQ(result, TPM2_SUCCESS);
//     std::this_thread::sleep_for(std::chrono::seconds(2)); // To see the file is being deleted or not.
//     result = tpmSeal.deleteSealKey(g_esys_context, context);
//     EXPECT_EQ(result, TPM2_SUCCESS);
//     TpmContext handle = dbService.getTpmContextByKeyNameAndDekAuth(context.keyName, context.dekAuth);
//     EXPECT_EQ(handle.indexValue, 0); // just to verify the record got removed from the table.
//     result = tpmAdmin.clearTpm(g_esys_context, "");
//     ASSERT_EQ(result, TPM2_SUCCESS);
// }

// /*delete existing sealed object with invalid object password*/
// TEST_F(SEAL_TEST, DeleteSealDataWithInvalidKeyPassword)
// {
//     TpmContext context;
//     context.ownerAuth = "";
//     context.srkAuth = "srkAuth";
//     context.dekAuth = "dekAuth";
//     context.fileName = seal_key_path;
//     context.keyName = "seal-key";
//     TPM2_RC result = tpmSeal.add(g_esys_context, context);
//     EXPECT_EQ(result, TPM2_SUCCESS);
//     context.dekAuth = "invalid-dekAuth";
//     result = tpmSeal.deleteSealKey(g_esys_context, context);
//     EXPECT_EQ(result, TPM2_INVALID_OBJECT_CREDENTIAL);
//     TpmContext handle = dbService.getTpmContextByKeyNameAndDekAuth(context.keyName, context.dekAuth);
//     EXPECT_EQ(handle.indexValue, 0);
//     result = tpmAdmin.clearTpm(g_esys_context, "");
//     ASSERT_EQ(result, TPM2_SUCCESS);
// }

// /*delete existing sealed object with invalid keyName*/
// TEST_F(SEAL_TEST, DeleteSealDataWithInvalidKeyName)
// {
//     TpmContext context;
//     context.ownerAuth = "";
//     context.srkAuth = "srkAuth";
//     context.dekAuth = "dekAuth";
//     context.fileName = seal_key_path;
//     context.keyName = "seal-key";
//     TPM2_RC result = tpmSeal.add(g_esys_context, context);
//     EXPECT_EQ(result, TPM2_SUCCESS);
//     context.keyName = "invalid-seal-key";
//     result = tpmSeal.deleteSealKey(g_esys_context, context);
//     EXPECT_EQ(result, TPM2_INVALID_OBJECT_CREDENTIAL);
//     TpmContext handle = dbService.getTpmContextByKeyNameAndDekAuth(context.keyName, context.dekAuth);
//     EXPECT_EQ(handle.indexValue, 0);
//     result = tpmAdmin.clearTpm(g_esys_context, "");
//     ASSERT_EQ(result, TPM2_SUCCESS);
// }

/*seal data with valid input*/
TEST_F(SEAL_TEST, SealDataWithValidInput)
{
    TpmContext context;
    string data = readFile(seal_key_path);
    context.data = data;
    context.ownerAuth = "";
    context.srkAuth = "srkAuth";
    context.dekAuth = "dekAuth";
    context.keyName = "seal-key";
    context.dataSize = data.size();
    TPM2_RC result = tpmSeal.add(g_esys_context, context); // seal data into the tpm.
    EXPECT_EQ(result, TPM2_SUCCESS);
    result = tpmAdmin.clearTpm(g_esys_context, "");
    ASSERT_EQ(result, TPM2_SUCCESS);
}

/*seal  data with invalid owner password*/
TEST_F(SEAL_TEST, SealDataWithInvalidOwnerAuth)
{
    TpmContext context;
    string data = readFile(seal_key_path);
    context.data = data;
    context.ownerAuth = "owner";
    context.srkAuth = "srkAuth";
    context.dekAuth = "dekAuth";
    context.keyName = "seal-key";
    context.dataSize = data.size();
    TPM2_RC result = tpmSeal.add(g_esys_context, context); // seal data into the tpm.
    EXPECT_EQ(result, TPM2_INVALID_HIERARCHY_AUTH);
    result = tpmAdmin.clearTpm(g_esys_context, "");
    ASSERT_EQ(result, TPM2_SUCCESS);
}

/*SEAL_DATA_WITH_TPM2_INVALID_ESYS_CONTEXT*/
TEST_F(SEAL_TEST, SealDataWithTpm2InvalidEsysContext)
{
    ESYS_CONTEXT *esys_context = NULL;
    TpmContext context;
    string data = readFile(seal_key_path);
    context.data = data;
    context.ownerAuth = "";
    context.srkAuth = "srkAuth";
    context.dekAuth = "dekAuth";
    context.keyName = "seal-key";
    context.dataSize = data.size();
    TPM2_RC result = tpmSeal.add(esys_context, context); // seal data into the tpm.
    EXPECT_EQ(result, TPM2_INVALID_ESYS_CONTEXT);
}

/*unseal data with valid input*/
TEST_F(SEAL_TEST, UnsealDataWithValidInput)
{
    TpmContext context;
    string data = readFile(seal_key_path);
    context.data = data;
    context.ownerAuth = "";
    context.srkAuth = "srkAuth";
    context.keyName = "sealkey";
    context.dekAuth = "dekAuth";
    context.dataSize = data.size();
    std::cout << "Data: " << context.data << std::endl;
    TPM2_RC result = tpmSeal.add(g_esys_context, context); // seal data into the tpm.
    EXPECT_EQ(result, TPM2_SUCCESS);
    result = tpmSeal.unSealSecret(g_esys_context, context);
    EXPECT_EQ(result, TPM2_SUCCESS);
    result = tpmAdmin.clearTpm(g_esys_context, ownerAuth);
    EXPECT_EQ(result, TPM2_SUCCESS);
}

/*unseal data with invalid object name input*/
TEST_F(SEAL_TEST, UnsealDataWithInvalidInput)
{
    TpmContext context;
    string data = readFile(seal_key_path);
    context.data = data;
    context.ownerAuth = "";
    context.srkAuth = "srkAuth";
    context.dekAuth = "dekAuth";
    context.keyName = "sealkey";
    context.dataSize = data.size();
    TPM2_RC result = tpmSeal.add(g_esys_context, context); // seal data into the tpm.
    EXPECT_EQ(result, TPM2_SUCCESS);
    context.keyName = "invalid-object-name";
    result = tpmSeal.unSealSecret(g_esys_context, context);
    EXPECT_EQ(result, TPM2_INVALID_OBJECT_CREDENTIAL);
    result = tpmAdmin.clearTpm(g_esys_context, ownerAuth);
    EXPECT_EQ(result, TPM2_SUCCESS);
}

/*seal data with no password*/
TEST_F(SEAL_TEST, SealDataWithNoPasswordProtection)
{
    TpmContext context;
    string data = readFile(seal_key_path);
    context.data = data;
    context.ownerAuth = "";
    context.srkAuth = "";
    context.dekAuth = "";
    context.keyName = "seal-key";
    context.dataSize = data.size();
    TPM2_RC result = tpmSeal.add(g_esys_context, context); // seal data into the tpm.
    EXPECT_EQ(result, TPM2_EMPTY_STRING);
    result = tpmAdmin.clearTpm(g_esys_context, "");
    ASSERT_EQ(result, TPM2_SUCCESS);
}

/*seal data with no object name*/
TEST_F(SEAL_TEST, SealDataWithNoKeyName)
{
    TpmContext context;
    string data = readFile(seal_key_path);
    context.data = data;
    context.ownerAuth = "";
    context.srkAuth = "srkAuth";
    context.dekAuth = "dekAuth";
    context.keyName = "";
    context.dataSize = data.size();
    TPM2_RC result = tpmSeal.add(g_esys_context, context); // seal data into the tpm.
    EXPECT_EQ(result, TPM2_EMPTY_STRING);
    result = tpmAdmin.clearTpm(g_esys_context, "");
    ASSERT_EQ(result, TPM2_SUCCESS);
}

/* seal data with key size is more that 256*/
TEST_F(SEAL_TEST, SealDataWithExceededKeySize)
{
    TpmContext context;
    const std::string &max_size_key_path = "/home/champ/Desktop/TPM/x_gtest_tpm/tmp/1024_private_key.pem";
    string data = readFile(max_size_key_path);
    context.data = data;
    context.ownerAuth = ownerAuth;
    context.srkAuth = "srkAuth";
    context.dekAuth = "dekAuth";
    context.keyName = "seal-key";
    context.dataSize = data.size();
    TPM2_RC result = tpmSeal.add(g_esys_context, context); // seal data into the tpm.
    EXPECT_EQ(result, TPM2_EXCEEDED_MAX_SEAL_BUFFER_SIZE);
    result = tpmAdmin.clearTpm(g_esys_context, ownerAuth);
    ASSERT_EQ(result, TPM2_SUCCESS);
}

/* seal data with key size is 0*/
TEST_F(SEAL_TEST, SealDataWithNoKeyData)
{
    SCOPED_TRACE("Seal data into the TPM with the empty key file, expecting this function will return TPM2_EMPTY_KEY_FILE");
    TpmContext context;
    const std::string &no_size_key_path = "/home/champ/Desktop/TPM/x_gtest_tpm/tmp/emptyfile";
    string data = readFile(no_size_key_path);
    context.data = data;
    context.ownerAuth = ownerAuth;
    context.srkAuth = "srkAuth";
    context.dekAuth = "dekAuth";
    context.keyName = "seal-key";
    context.dataSize = data.size();
    TPM2_RC result = tpmSeal.add(g_esys_context, context); // seal data into the tpm.
    EXPECT_EQ(result, TPM2_EMPTY_STRING);
    result = tpmAdmin.clearTpm(g_esys_context, ownerAuth);
    ASSERT_EQ(result, TPM2_SUCCESS);
}

/*seal data with invalid key file*/
TEST_F(SEAL_TEST, SealDataWithInvalidKeyFile)
{
    TpmContext context;
    string data = readFile(invalid_seal_key_path);
    context.data = data;
    context.ownerAuth = "";
    context.srkAuth = "srkAuth";
    context.dekAuth = "dekAuth";
    context.keyName = "seal-key";
    context.dataSize = data.size();
    TPM2_RC result = tpmSeal.add(g_esys_context, context); // seal data into the tpm.
    EXPECT_EQ(result, TPM2_EMPTY_STRING);
    result = tpmAdmin.clearTpm(g_esys_context, "");
    ASSERT_EQ(result, TPM2_SUCCESS);
}
