#ifndef TPM_SEAL_HPP
#define TPM_SEAL_HPP
#include "TpmEntity.hpp"
#include "KeyHandler.hpp"
#include "TpmKeyService.hpp"
using namespace SCM::Tpm;

class TpmSeal
{
public:
    TpmSeal() : db(std::make_unique<DbService>(TPM_CONTEXT_DB_PATH))
    {
    }

    TPM2_RC add(ESYS_CONTEXT *esys_context, TpmContext &context)
    {
        context.type = 0;
        if (esys_context == NULL)
        {
            LOG_ERROR("TPM2_INVALID_ESYS_CONTEXT");
            return TPM2_INVALID_ESYS_CONTEXT;
        }

        TPM2_RC result = validateTpmContext(context);
        if (result != TPM2_SUCCESS)
        {
            return result;
        }
        int response = TPM2_INVALID_OBJECT_CREDENTIAL;
        // int response = TPM2_SUCCESS;
        // int sealIndex = TPM2_PERSISTENT_FIRST;
        int index = 0;
        response = db->getTpmContextByName(context.keyName);
        if (response == TPM2_INVALID_DATABASE)
        {
            return response;
        }
        else if (response == TPM2_DUPLICATE_VALUE)
        {
            LOG_ERROR("Key alread stored in tpm with this name", context.keyName);
            return response;
        }
        while (index < 3)
        {
            response = db->getTpmContextByIndex(index);
            if (response == TPM2_DUPLICATE_VALUE)
            {
                break;
            }
            index++;
        }

        if (response == TPM2_DUPLICATE_VALUE && index >= 3)
        {
            return response;
        }
        else
        {
            context.index = index;
            context.fileName = Common::generateKeyName(context.type, context.index);
            DEBUG("The created Path : " + context.fileName);
            return addHash(esys_context, context);
        }
    }

    TPM2_RC unSealSecret(ESYS_CONTEXT *esys_context, TpmContext &context)
    {
        context.type = 0;
        if (esys_context == NULL)
        {
            return TPM2_INVALID_ESYS_CONTEXT;
        }
        int result = db->getTpmContextByKeyNameAndDekAuth(context);
        if (result != SCM::Tpm::TPM2_DUPLICATE_VALUE)
        {
            LOG_ERROR("Invalid object name and password:", context.keyName, context.dekAuth);
            return SCM::Tpm::TPM2_INVALID_OBJECT_CREDENTIAL;
        }
        else
        {
            context.fileName = Common::generateKeyName(context.type, context.index);
        }
        TPM2_RC response = TPM2_SUCCESS;
        ESYS_TR loadedKeyHandle = ESYS_TR_NONE;
        ESYS_TR session = ESYS_TR_NONE;
        TPMT_SYM_DEF symmteric = {.algorithm = TPM2_ALG_NULL};
        TPM2B_SENSITIVE_DATA *outData = NULL;
        uint8_t buffer[TPM2_SEAL_BUFFER_SIZE];
        size_t buffer_size;

        TPM2B_AUTH ownerAuth =
            {
                .size = static_cast<UINT16>(context.ownerAuth.size()),
                .buffer = {0}};
        std::memcpy(ownerAuth.buffer, context.ownerAuth.c_str(), context.ownerAuth.size());

        TPM2B_AUTH authValue =
            {
                .size = static_cast<UINT16>(context.dekAuth.size()),
                .buffer = {0}};

        std::memcpy(authValue.buffer, context.dekAuth.c_str(), context.dekAuth.size());

        std::ifstream file(context.fileName, std::ios::binary);
        if (!file)
        {
            LOG_ERROR("No such file to read dek handle: ", context.fileName);
            return TPM2_INVALID_FILE; // Or appropriate error handling
        }
        else
        {
            file.read(reinterpret_cast<char *>(buffer), TPM2_SEAL_BUFFER_SIZE);
            std::streamsize size = file.gcount();
            buffer_size = static_cast<size_t>(size);
            file.close();
        }

        try
        {
            response = Esys_TR_Deserialize(esys_context, buffer, buffer_size, &loadedKeyHandle);

            handleTPMResponse(response, "Failed to deserialize dek handle.");

            response = Esys_StartAuthSession(esys_context, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, NULL, TPM2_SE_HMAC, &symmteric, TPM2_ALG_SHA256, &session);
            handleTPMResponse(response, "Failed to start auth session.");

            response = Esys_TR_SetAuth(esys_context, ESYS_TR_RH_OWNER, &ownerAuth);
            handleTPMResponse(response, "Failed to owner auth value.");

            response = Esys_TR_SetAuth(esys_context, loadedKeyHandle, &authValue);
            handleTPMResponse(response, "Failed to set key auth value.");

            response = Esys_Unseal(esys_context, loadedKeyHandle, session, ESYS_TR_NONE, ESYS_TR_NONE, &outData);
            handleTPMResponse(response, "failed to unseal the secret.");

            context.data = reinterpret_cast<char *>(outData->buffer);
            context.dataSize = outData->size;

            response = Esys_FlushContext(esys_context, session);

            handleTPMResponse(response, "failed to flush session");

            Esys_Free(outData);
        }
        catch (const TPMException &ex)
        {
            LOG_ERROR(ex.what());
            if (session != ESYS_TR_NONE && Esys_FlushContext(esys_context, session) != TPM2_SUCCESS)
            {
                LOG_ERROR("Failed to clear session context.");
            }
            Esys_Free(outData);
        }
        return response;
    }

    // void unseal_data(ESYS_CONTEXT *esys_context, TpmContext &tpmContext)
    // {
    //     tpmContext.type = 0;
    //     tpmContext.fileName = Common::generateKeyName(tpmContext.type, tpmContext.index);
    //     TPMS_CONTEXT context;
    //     read_context_from_file(&context, tpmContext.fileName);

    //     ESYS_TR primaryHandle;
    //     TPM2_RC ret = Esys_ContextLoad(esys_context, &context, &primaryHandle);
    //     if (ret != TSS2_RC_SUCCESS)
    //     {
    //         fprintf(stderr, "Esys_ContextLoad failed: 0x%x\n", ret);
    //         exit(1);
    //     }

    //     TPM2B_SENSITIVE_DATA *outData;
    //     ret = Esys_Unseal(
    //         esys_context,
    //         primaryHandle,
    //         ESYS_TR_PASSWORD,
    //         ESYS_TR_NONE,
    //         ESYS_TR_NONE,
    //         &outData);
    //     if (ret != TSS2_RC_SUCCESS)
    //     {
    //         fprintf(stderr, "Esys_Unseal failed: 0x%x\n", ret);
    //         exit(1);
    //     }

    //     Esys_Finalize(&esys_context);

    //     // Use outData as needed
    // }
    /* TPM2_RC deleteSealKey(ESYS_CONTEXT *esys_context, TpmContext &context)
     {
         if (esys_context == NULL)
         {
             return TPM2_INVALID_ESYS_CONTEXT;
         }
         int result = db->getTpmContextByKeyNameAndDekAuth(context);
         if (result != SCM::SUCCESS)
         {
             LOG_ERROR("Invalid object name and password:", context.keyName, context.dekAuth);
             return SCM::Tpm::TPM2_INVALID_OBJECT_CREDENTIAL;
         }
         else
         {
             context.fileName = Common::generateKeyName(context.type, context.index);
         }

         TPM2_RC response = TPM2_SUCCESS;
         ESYS_TR objectHandle = ESYS_TR_NONE;
         ESYS_TR session = ESYS_TR_NONE;
         TPMT_SYM_DEF symmteric = {.algorithm = TPM2_ALG_NULL};
         uint8_t buffer[TPM2_SEAL_BUFFER_SIZE];
         FILE *file = NULL;
         size_t buffer_size;
         std::ifstream file(context.fileName, std::ios::binary);
         if (!file)
         {
             LOG_ERROR("No such file to read dek handle: ", context.fileName);
             return TPM2_INVALID_FILE; // Or appropriate error handling
         }
         else
         {
             file.read(reinterpret_cast<char *>(buffer), TPM2_SEAL_BUFFER_SIZE);
             std::streamsize size = file.gcount();
             buffer_size = static_cast<size_t>(size);
             file.close();
         }

         try
         {
             response = Esys_TR_Deserialize(esys_context, buffer, buffer_size, &objectHandle);
             handleTPMResponse(response, "Failed to DE-Serialize the SRK handle in Seal key deletion.");
             response = Esys_StartAuthSession(esys_context, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, NULL, TPM2_SE_HMAC, &symmteric, TPM2_ALG_SHA256, &session);
             handleTPMResponse(response, "Failed to start auth session.");
             response = Esys_EvictControl(esys_context, ESYS_TR_RH_OWNER, objectHandle,
                                          session, ESYS_TR_NONE, ESYS_TR_NONE,
                                          handle.indexValue, &objectHandle);
             handleTPMResponse(response, "Failed to evict the handle");
             response = Esys_FlushContext(esys_context, session);
         }
         catch (const TPMException &e)
         {
             std::cerr << e.what() << '\n';
         }

         response = Esys_EvictControl(esys_context, ESYS_TR_RH_OWNER, objectHandle,
                                      session, ESYS_TR_NONE, ESYS_TR_NONE,
                                      handle.indexValue, &objectHandle);

         if (response != TPM2_SUCCESS)
         {
             LOG(TPM2_ERROR_STRING, "Failed to evict the handle {%d}", handle.indexValue);
             goto error;
         }

         response = Esys_FlushContext(esys_context, session);
         if (response != TPM2_SUCCESS)
         {
             LOG(TPM2_ERROR_STRING, "failed to flush session handle.");
             goto error;
         }

         if (!(deleteHandle(seal.objectName, seal.dekAuth)))
         {
             goto error;
         }
         remove(filepath);
         char *childObjectName = malloc(strlen(seal.objectName) + sizeof(DEK_KEY));
         strcpy(childObjectName, seal.objectName);
         strcat(childObjectName, DEK_KEY);
         LOG(TPM2_INFO_STRING, "DEK key : %s\n", childObjectName);
         snprintf(filepath, sizeof(filepath), "%d", handle.indexValue + handle.dataSize);
         LOG(TPM2_INFO_STRING, "The DEK file name : %s\n", filepath);
         if (!(deleteHandle(childObjectName, seal.dekAuth)))
         {
             free(childObjectName);
             goto error;
         }
         remove(filepath);
         free(childObjectName);

         return response;
     error:
         if (session != ESYS_TR_NONE && Esys_FlushContext(esys_context, session) != TPM2_SUCCESS)
         {
             LOG(TPM2_ERROR_STRING, "Failed to clear session context.");
         }
         return response;
     }
 */
private:
    TPM2_RC addHash(ESYS_CONTEXT *esys_context, TpmContext &context)
    {
        TPM2_RC response = TPM2_SUCCESS;
        try
        {
            response = keyService.createSRK(esys_context, context);
            handleTPMResponse(response, "Primary key creation failed.");
        }
        catch (std::exception &ex)
        {
            LOG_ERROR("Failed to seal secret into TPM, ", ex.what());
        }
        return response;
    }

    TPM2_RC validateTpmContext(const TpmContext &context)
    {
        if (context.dekAuth.empty() || context.data.empty() || context.keyName.empty())
        {
            DEBUG("TPM2_EMPTY_STRING");
            return TPM2_EMPTY_STRING;
        }

        return TPM2_SUCCESS;
    }

private:
    std::unique_ptr<DbService> db;
    KeyHandler key;
    TpmKeyService keyService;
};

#endif