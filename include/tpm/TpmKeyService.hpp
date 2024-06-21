#ifndef KEY_SERVICE_HPP
#define KEY_SERVICE_HPP

#include "TpmException.hpp"
#include "TpmEntity.hpp"
#include "db/DbService.hpp"

class TpmKeyService
{
public:
    TpmKeyService() : db(std::make_unique<DbService>(SCM::Tpm::DEFAULT_DB_PATH)) {}
    TPM2_RC createSRK(ESYS_CONTEXT *esys_context, TpmContext &tpmContext)
    {
        TPM2_RC response = TPM2_RC_SUCCESS;
        ESYS_TR objectHandle = ESYS_TR_NONE;
        ESYS_TR session = ESYS_TR_NONE;
        TPMT_SYM_DEF symmetric = {.algorithm = TPM2_ALG_NULL};

        TPM2B_PUBLIC *outPublic = nullptr;
        TPM2B_CREATION_DATA *creationData = nullptr;
        TPM2B_DIGEST *creationHash = nullptr;
        TPMT_TK_CREATION *creationTicket = nullptr;

        TPM2B_SENSITIVE_CREATE inSensitive = {
            .size = 0,
            .sensitive = {
                .userAuth = {
                    .size = static_cast<UINT16>(tpmContext.srkAuth.size()),
                    .buffer = {0},
                },
                .data = {.size = 0, .buffer = {0}}}};

        std::memcpy(inSensitive.sensitive.userAuth.buffer, tpmContext.srkAuth.c_str(), tpmContext.srkAuth.size());

        TPM2B_PUBLIC inPublic;
        inPublic.size = 0;
        inPublic.publicArea.type = TPM2_ALG_RSA;
        inPublic.publicArea.nameAlg = TPM2_ALG_SHA256;
        inPublic.publicArea.objectAttributes = TPMA_OBJECT_USERWITHAUTH |
                                               TPMA_OBJECT_RESTRICTED |
                                               TPMA_OBJECT_DECRYPT |
                                               TPMA_OBJECT_FIXEDTPM |
                                               TPMA_OBJECT_FIXEDPARENT |
                                               TPMA_OBJECT_SENSITIVEDATAORIGIN;
        inPublic.publicArea.authPolicy.size = 0;
        inPublic.publicArea.parameters.rsaDetail.symmetric.algorithm = TPM2_ALG_AES;
        inPublic.publicArea.parameters.rsaDetail.symmetric.keyBits.aes = 128;
        inPublic.publicArea.parameters.rsaDetail.symmetric.mode.aes = TPM2_ALG_CFB;
        inPublic.publicArea.parameters.rsaDetail.scheme.scheme = TPM2_ALG_NULL;
        inPublic.publicArea.parameters.rsaDetail.keyBits = 2048;
        inPublic.publicArea.parameters.rsaDetail.exponent = 0;
        inPublic.publicArea.unique.rsa.size = 0;
        memset(inPublic.publicArea.unique.rsa.buffer, 0, sizeof(inPublic.publicArea.unique.rsa.buffer));

        TPM2B_DATA outsideInfo = {
            .size = 0,
            .buffer = {0},
        };

        TPML_PCR_SELECTION creationPCR = {
            .count = 0,
        };

        TPM2B_AUTH authValue = {
            .size = static_cast<UINT16>(tpmContext.ownerAuth.size()),
            .buffer = {0}};
        std::memcpy(authValue.buffer, tpmContext.ownerAuth.c_str(), tpmContext.ownerAuth.size());

        try
        {
            response = Esys_StartAuthSession(esys_context, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, nullptr, TPM2_SE_HMAC, &symmetric, TPM2_ALG_SHA256, &session);
            handleTPMResponse(response, "Failed to start auth session");

            response = Esys_TR_SetAuth(esys_context, ESYS_TR_RH_OWNER, &authValue);
            handleTPMResponse(response, "Authentication failure");

            response = Esys_CreatePrimary(esys_context, ESYS_TR_RH_OWNER, session, ESYS_TR_NONE, ESYS_TR_NONE, &inSensitive, &inPublic, &outsideInfo, &creationPCR, &objectHandle, &outPublic, &creationData, &creationHash, &creationTicket);
            handleTPMResponse(response, "Creating Primary key failed");

            response = Esys_FlushContext(esys_context, session);
            handleTPMResponse(response, "ERROR during flush session context");

            response = createDEK(esys_context, objectHandle, tpmContext);
            handleTPMResponse(response, "Creating DEK failed");

            response = Esys_FlushContext(esys_context, objectHandle);
            handleTPMResponse(response, "ERROR during flush object context");

            Esys_Free(outPublic);
            Esys_Free(creationData);
            Esys_Free(creationHash);
            Esys_Free(creationTicket);
        }
        catch (const TPMException &ex)
        {
            if (objectHandle != ESYS_TR_NONE)
            {
                Esys_FlushContext(esys_context, objectHandle);
            }
            if (session != ESYS_TR_NONE)
            {
                Esys_FlushContext(esys_context, session);
            }
            Esys_Free(outPublic);
            Esys_Free(creationData);
            Esys_Free(creationHash);
            Esys_Free(creationTicket);
            LOG_ERROR(ex.what());
            // return ex.errorCode;
        }
        return response;
    }

    TPM2_RC createDEK(ESYS_CONTEXT *esys_context, ESYS_TR srk_context, TpmContext &tpmContext)
    {
        ESYS_TR loadedKeyHandle = ESYS_TR_NONE;
        ESYS_TR persistent_handle = ESYS_TR_NONE;
        ESYS_TR session = ESYS_TR_NONE;
        TPMT_SYM_DEF symmetric = {.algorithm = TPM2_ALG_NULL};
        TPM2_RC response = TPM2_SUCCESS;
        TPM2B_PUBLIC *outPublic = NULL;
        TPM2B_PRIVATE *outPrivate = NULL;
        TPM2B_CREATION_DATA *creationData = NULL;
        TPM2B_DIGEST *creationHash = NULL;
        TPMT_TK_CREATION *creationTicket = NULL;
        TPM2B_SENSITIVE_DATA *outData = NULL;
        size_t buffer_size;
        uint8_t *buffer = nullptr;

        TPM2B_SENSITIVE_CREATE inSensitive = {
            .size = 0,
            .sensitive = {
                .userAuth = {
                    .size = static_cast<UINT16>(tpmContext.dekAuth.size()),
                    .buffer = {0},
                },
                .data = {.size = static_cast<UINT16>(tpmContext.dataSize), .buffer = {0}}}};

        std::memcpy(inSensitive.sensitive.userAuth.buffer, tpmContext.dekAuth.c_str(), tpmContext.dekAuth.size());
        std::memcpy(inSensitive.sensitive.data.buffer, tpmContext.data.c_str(), tpmContext.dataSize);

        TPM2B_DIGEST keyedHash =
            {
                .size = 0,
                .buffer = {0},
            };

        TPM2B_PUBLIC inPublic;
        inPublic.size = 0;
        inPublic.publicArea.type = TPM2_ALG_KEYEDHASH;
        inPublic.publicArea.nameAlg = TPM2_ALG_SHA256;
        inPublic.publicArea.objectAttributes = TPMA_OBJECT_USERWITHAUTH |
                                               TPMA_OBJECT_FIXEDTPM |
                                               TPMA_OBJECT_FIXEDPARENT;
        inPublic.publicArea.authPolicy.size = 0;
        inPublic.publicArea.parameters.keyedHashDetail.scheme.scheme = TPM2_ALG_NULL;
        inPublic.publicArea.parameters.keyedHashDetail.scheme.details.hmac.hashAlg = TPM2_ALG_SHA256;
        inPublic.publicArea.unique.keyedHash = keyedHash;

        TPM2B_DATA outsideInfo = {
            .size = 0,
            .buffer = {0},
        };

        TPML_PCR_SELECTION creationPCR = {
            .count = 0,
        };

        TPM2B_AUTH primaryAuth = {
            .size = static_cast<UINT16>(tpmContext.srkAuth.size()),
            .buffer = {0},
        };

        std::memcpy(primaryAuth.buffer, tpmContext.srkAuth.c_str(), tpmContext.srkAuth.size());

        TPM2_HANDLE permanentHandle = TPM2_PERSISTENT_FIRST + tpmContext.index;

        try
        {
            response = Esys_StartAuthSession(esys_context, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, nullptr, TPM2_SE_HMAC, &symmetric, TPM2_ALG_SHA256, &session);
            handleTPMResponse(response, "Failed to start auth session.");

            response = Esys_Create(esys_context, srk_context, session, ESYS_TR_NONE, ESYS_TR_NONE, &inSensitive, &inPublic, &outsideInfo, &creationPCR, &outPrivate, &outPublic, &creationData, &creationHash, &creationTicket);
            handleTPMResponse(response, "Error creating DEK");

            response = Esys_Load(esys_context, srk_context, session, ESYS_TR_NONE, ESYS_TR_NONE, outPrivate, outPublic, &loadedKeyHandle);
            handleTPMResponse(response, "Error load key into the TPM");

            response = Esys_EvictControl(esys_context, ESYS_TR_RH_OWNER, loadedKeyHandle,
                                         session, ESYS_TR_NONE, ESYS_TR_NONE,
                                         permanentHandle, &persistent_handle);
            handleTPMResponse(response, "Unable to save the authorization context");

            response = Esys_TR_Serialize(esys_context, loadedKeyHandle, &buffer, &buffer_size);
            handleTPMResponse(response, "Error serializing DEK handle.");

            std::ofstream outFile(tpmContext.fileName, std::ios::binary);
            // if (outFile.is_open())
            // {
            //     outFile.write(reinterpret_cast<char *>(buffer), buffer_size);
            //     outFile.close();
            // }
            if (!outFile)
            {
                LOG_ERROR("No such file to write DEK handle:", tpmContext.fileName);
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
            std::cout << "Response: " << response << '\n';

            response = Esys_FlushContext(esys_context, session);
            handleTPMResponse(response, "Unable to flush handle");
            Esys_Free(outPublic);
            Esys_Free(creationData);
            Esys_Free(creationHash);
            Esys_Free(creationTicket);
            Esys_Free(outData);
        }
        catch (const TPMException &ex)
        {
            if (session != ESYS_TR_NONE)
            {
                Esys_FlushContext(esys_context, session);
            }
            LOG_ERROR(ex.what());
            Esys_Free(outPublic);
            Esys_Free(creationData);
            Esys_Free(creationHash);
            Esys_Free(creationTicket);
            Esys_Free(outData);
        }
        return response;
    }

private:
    std::unique_ptr<DbService> db;
};

#endif