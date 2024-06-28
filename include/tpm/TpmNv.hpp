#ifndef TPM_NV_HPPP
#define TPM_NV_HPPP
#include "util/Common.hpp"
#include "TpmEntity.hpp"
#include "TpmException.hpp"
#include "db/DbService.hpp"

using namespace SCM::Tpm;
class TpmNv
{
public:
    TpmNv() : db(std::make_unique<DbService>(TPM_CONTEXT_DB_PATH)) {}

    TPM2_RC nvDelete(ESYS_CONTEXT *esys_context, TpmContext &context)
    {
        context.type = 1;
        if (esys_context == nullptr)
        {
            return TPM2_INVALID_ESYS_CONTEXT;
        }
        TPM2_RC response = TPM2_SUCCESS;
        ESYS_TR nvHandle = ESYS_TR_NONE;
        int result = db->getTpmContextByKeyNameAndDekAuth(context);
        if (result != SCM::SUCCESS)
        {
            return result;
        }
        context.fileName = Common::generateKeyName(1, context.index);
        uint8_t buffer[BUFFER_SIZE];
        size_t buffer_size;

        TPM2B_AUTH owner_auth =
            {
                .size = static_cast<UINT16>(context.ownerAuth.size()),
                .buffer = {0}};

        std::memcpy(owner_auth.buffer, context.ownerAuth.c_str(), context.ownerAuth.size());

        TPM2B_AUTH auth =
            {
                .size = static_cast<UINT16>(context.dekAuth.size()),
                .buffer = {0}};

        std::memcpy(auth.buffer, context.dekAuth.c_str(), context.dekAuth.size());

        std::ifstream file(context.fileName, std::ios::binary);
        if (!file)
        {
            LOG_ERROR("No such file to read authorization handle: ", context.fileName);
            return TPM2_INVALID_FILE;
        }
        else
        {
            file.read(reinterpret_cast<char *>(buffer), BUFFER_SIZE);
            std::streamsize size = file.gcount();
            buffer_size = static_cast<size_t>(size);
            file.close();
        }

        try
        {
            response = Esys_TR_Deserialize(esys_context, buffer, buffer_size, &nvHandle);
            handleTPMResponse(response, "NV DE-Serialization failed.");
            response = Esys_TR_SetAuth(esys_context, nvHandle, &owner_auth);
            handleTPMResponse(response, "Failed to set owner auth value.");
            response = Esys_TR_SetAuth(esys_context, nvHandle, &auth);
            handleTPMResponse(response, "Failed to set context auth value.");
            response = Esys_NV_UndefineSpace(esys_context, ESYS_TR_RH_OWNER, nvHandle, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE);
            handleTPMResponse(response, "Failed to delete index from NVRam.");
            response = db->deleteHandle(context);
            if (response == SCM::SUCCESS)
            {
                DEBUG("Record removed from database.");
                string command = "rm -f " + context.fileName;
                (void)std::system(command.c_str());
            }
        }
        catch (const TPMException &e)
        {
            LOG_ERROR(e.what());
            return e.errorCode;
        }
        return response == 1 ? 0 : response;
    }

    TPM2_RC nvRead(ESYS_CONTEXT *esys_context, TpmContext &context)
    {
        context.type = 1;
        if (esys_context == NULL)
        {
            return TPM2_INVALID_ESYS_CONTEXT;
        }
        int result = TPM2_SUCCESS;
        result = db->getTpmContextByKeyNameAndDekAuth(context);
        if (result != SCM::SUCCESS)
        {
            return result;
        }
        context.fileName = Common::generateKeyName(1, context.index);
        TPM2_RC response = TPM2_SUCCESS;
        ESYS_TR nvHandle = ESYS_TR_NONE;
        TPM2B_MAX_NV_BUFFER *nv_test_data2 = NULL;
        uint8_t buffer[BUFFER_SIZE];
        size_t buffer_size;

        std::ifstream file(context.fileName, std::ios::binary);
        if (!file)
        {
            LOG_ERROR("No such file to read dek handle: ", context.fileName);
            return TPM2_INVALID_FILE; // Or appropriate error handling
        }
        else
        {
            file.read(reinterpret_cast<char *>(buffer), BUFFER_SIZE);
            std::streamsize size = file.gcount();
            buffer_size = static_cast<size_t>(size);
            file.close();
        }

        TPM2B_AUTH owner_auth =
            {
                .size = static_cast<UINT16>(context.ownerAuth.size()),
                .buffer = {0}};

        std::memcpy(owner_auth.buffer, context.ownerAuth.c_str(), context.ownerAuth.size());

        TPM2B_AUTH auth =
            {
                .size = static_cast<UINT16>(context.dekAuth.size()),
                .buffer = {0}};

        std::memcpy(auth.buffer, context.dekAuth.c_str(), context.dekAuth.size());
        try
        {
            response = Esys_TR_Deserialize(esys_context, buffer, buffer_size, &nvHandle);
            handleTPMResponse(response, "DE-Serialization failed in NV Read.");
            response = Esys_TR_SetAuth(esys_context, nvHandle, &owner_auth);
            handleTPMResponse(response, "Failed to set owner auth value.");
            response = Esys_TR_SetAuth(esys_context, nvHandle, &auth);
            handleTPMResponse(response, "Failed to set context auth value.");
            response = Esys_NV_Read(esys_context, nvHandle, nvHandle, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE, static_cast<UINT16>(context.dataSize), 0, &nv_test_data2);
            handleTPMResponse(response, "Failed to read data from NV RAM.");
            INFO("The data from NV RAM : ", nv_test_data2->buffer);
            context.data = reinterpret_cast<char *>(nv_test_data2->buffer);
            context.dataSize = nv_test_data2->size;
            Esys_Free(nv_test_data2);
        }
        catch (const TPMException &e)
        {
            LOG_ERROR(e.what());
            Esys_Free(nv_test_data2);
            return e.errorCode;
        }
        return response;
    }

    TPM2_RC addKey(ESYS_CONTEXT *esys_context, TpmContext &tpmContext)
    {
        TPM2_RC response = TPM2_SUCCESS;
        ESYS_TR nvHandle = ESYS_TR_NONE;
        uint8_t buffer[BUFFER_SIZE];
        uint8_t key_buffer[TPM2_MAX_NV_BUFFER_SIZE];
        UINT16 key_buffe_size;
        uint8_t *outBuffer = NULL;
        size_t buffer_size;
        UINT16 offset = 0;

        TPM2B_AUTH auth = {
            .size = static_cast<UINT16>(tpmContext.dekAuth.size()),
            .buffer = {0}};
        std::memcpy(auth.buffer, tpmContext.dekAuth.c_str(), tpmContext.dekAuth.size());

        std::memcpy(key_buffer, tpmContext.data.c_str(), tpmContext.dataSize);

        key_buffe_size = static_cast<UINT16>(tpmContext.dataSize);

        INFO("Key buffer size: ", tpmContext.dataSize);

        TPM2B_MAX_NV_BUFFER nv_test_data =
            {
                .size = key_buffe_size,
                .buffer = {0}};

        std::memcpy(nv_test_data.buffer, tpmContext.data.c_str(), tpmContext.dataSize);

        try
        {
            response = nvDefineSpace(esys_context, tpmContext);
            handleTPMResponse(response, "Unable to define nv space");
            std::ifstream file(tpmContext.fileName, std::ios::binary);
            if (!file)
            {
                LOG_ERROR("No such file to read dek handle: ", tpmContext.fileName);
                return TPM2_INVALID_FILE; // Or appropriate error handling
            }
            else
            {
                file.read(reinterpret_cast<char *>(buffer), TPM2_SEAL_BUFFER_SIZE);
                std::streamsize size = file.gcount();
                buffer_size = static_cast<size_t>(size);
                file.close();
            }
            response = Esys_TR_Deserialize(esys_context, buffer, buffer_size, &nvHandle);
            handleTPMResponse(response, "Deserialization failed in NV Write.");
            response = Esys_TR_SetAuth(esys_context, nvHandle, &auth);
            handleTPMResponse(response, "Failed to set auth value.");
            response = Esys_NV_Write(esys_context,
                                     nvHandle,
                                     nvHandle,
                                     ESYS_TR_PASSWORD,
                                     ESYS_TR_NONE,
                                     ESYS_TR_NONE,
                                     &nv_test_data,
                                     offset);
            handleTPMResponse(response, "Failed to serialize nv handle.");
            std::ofstream outFile(tpmContext.fileName, std::ios::binary);
            if (!outFile)
            {
                LOG_ERROR("No such file to write Nv handle::", tpmContext.fileName);
            }
            else
            {
                outFile.write(reinterpret_cast<char *>(buffer), buffer_size);
                if (!outFile)
                {
                    LOG_ERROR("Failed to write to file: ", tpmContext.fileName);
                }
                outFile.close();
            }

            response = db->addTpmContext(tpmContext);
            if (response == SCM::Tpm::TPM2_SUCCESS)
            {
                INFO("TpmContext saved in DB.");
            }
            else
            {
                LOG_ERROR("Unable to save Tpmcontext in DB.");
            }
        }
        catch (const TPMException &e)
        {
            LOG_ERROR(e.what());
            return e.errorCode;
        }
        return response;
    }

    TPM2_RC nvProxy(ESYS_CONTEXT *esys_context, TpmContext &context)
    {
        context.type = 1;
        int result = 0;
        if((result = validateTpmContext(context))!= TPM2_SUCCESS)
        {
            return result;
        }

        if (esys_context == nullptr)
        {
            return TPM2_INVALID_ESYS_CONTEXT;
        }
        UINT16 response = TPM2_INVALID_OBJECT_CREDENTIAL;
        int index = 0;
        response = db->getTpmContextByName(context.keyName);
        if (response == TPM2_INVALID_DATABASE)
        {
            return response;
        }
        else if (response == TPM2_DUPLICATE_VALUE)
        {
            LOG_ERROR("Key alread store in tpm with this name");
            return response;
        }
        while (index >= 0 && index < TPM2_PERSISTENT_KEY_LIMIT)
        {
            response = db->getTpmContextByIndex(index);
            if (response == TPM2_SUCCESS)
            {
                break;
            }
            index++;
        }

        if (response == TPM2_SUCCESS && index >= TPM2_PERSISTENT_KEY_LIMIT)
        {
            return response;
        }
        context.index = index;
        context.fileName = Common::generateKeyName(1, context.index);
        DEBUG("The created Path : " + context.fileName);
        return addKey(esys_context, context);
    }

    TPM2_RC nvDefineSpace(ESYS_CONTEXT *esys_context, TpmContext &context)
    {
        TPM2_RC response = TPM2_SUCCESS;
        ESYS_TR nvHandle = ESYS_TR_NONE;
        FILE *file = NULL;
        uint8_t *buffer = NULL;
        size_t buffer_size;

        TPM2B_AUTH owner_auth =
            {
                .size = static_cast<UINT16>(context.ownerAuth.size()),
                .buffer = {0}};

        std::memcpy(owner_auth.buffer, context.ownerAuth.c_str(), context.ownerAuth.size());

        TPM2B_AUTH auth =
            {
                .size = static_cast<UINT16>(context.dekAuth.size()),
                .buffer = {0}};

        std::memcpy(auth.buffer, context.dekAuth.c_str(), context.dekAuth.size());

        TPM2B_NV_PUBLIC publicInfo;
        publicInfo.size = 0;
        publicInfo.nvPublic.nvIndex = TPM2_NV_INDEX_FIRST + context.index;
        publicInfo.nvPublic.nameAlg = TPM2_ALG_SHA256;
        publicInfo.nvPublic.attributes = TPMA_NV_OWNERWRITE |
                                         TPMA_NV_AUTHWRITE |
                                         TPMA_NV_WRITE_STCLEAR |
                                         TPMA_NV_READ_STCLEAR |
                                         TPMA_NV_AUTHREAD |
                                         TPMA_NV_OWNERREAD;
        publicInfo.nvPublic.authPolicy.size = 0;
        publicInfo.nvPublic.dataSize = static_cast<UINT16>(context.dataSize);

        try
        {
            response = Esys_TR_SetAuth(esys_context, ESYS_TR_RH_OWNER, &owner_auth);
            handleTPMResponse(response, "Failed to set auth value");
            response = Esys_NV_DefineSpace(esys_context,
                                           ESYS_TR_RH_OWNER,
                                           ESYS_TR_PASSWORD,
                                           ESYS_TR_NONE,
                                           ESYS_TR_NONE,
                                           &auth,
                                           &publicInfo,
                                           &nvHandle);
            handleTPMResponse(response, "Failed to define space in NV Define");
            response = Esys_TR_Serialize(esys_context, nvHandle, &buffer, &buffer_size);
            handleTPMResponse(response, "Failed to serialize the NV RAM data.");
            context.fileName = Common::generateKeyName(1, context.index);
            std::ofstream outFile(context.fileName, std::ios::binary);
            if (!outFile)
            {
                LOG_ERROR("No such file to write Nv handle::", context.fileName);
            }
            else
            {
                outFile.write(reinterpret_cast<char *>(buffer), buffer_size);
                if (!outFile)
                {
                    LOG_ERROR("Failed to write to file: ", context.fileName);
                }
                outFile.close();
            }
        }
        catch (TPMException &ex)
        {
            LOG_ERROR(ex.what());
            return ex.errorCode;
        }
        return response;
    }

private:
    TPM2_RC validateTpmContext(const TpmContext &context)
    {
        if (context.dekAuth.empty() || context.keyName.empty())
        {
            DEBUG("TPM2_INVALID_CONTEXT");
            return TPM2_EMPTY_STRING;
        }
        if(context.data.empty() || context.dataSize <= 0)
        {
            LOG_ERROR("TPM2_EMPTY_KEY_FILE");
            return TPM2_EMPTY_KEY_FILE;
        }
        return TPM2_SUCCESS;
    }

private:
    std::unique_ptr<DbService> db;
};

#endif