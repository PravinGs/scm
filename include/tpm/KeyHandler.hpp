#ifndef KEY_HANDLER_HPP
#define KEY_HANDLER_HPP

#include "util/Common.hpp"
using namespace SCM::Tpm;
class KeyHandler
{
public:
    TPM2_RC createSrk(ESYS_CONTEXT *esys_context, TpmContext &context)
    {
        TPM2_RC response = TPM2_SUCCESS;
        ESYS_TR objectHandle = ESYS_TR_NONE;
        ESYS_TR session = ESYS_TR_NONE;
        TPMT_SYM_DEF symmteric = {.algorithm = TPM2_ALG_NULL};

        TPM2B_PUBLIC *outPublic = NULL;
        TPM2B_CREATION_DATA *creationData = NULL;
        TPM2B_DIGEST *creationHash = NULL;
        TPMT_TK_CREATION *creationTicket = NULL;

        TPM2B_SENSITIVE_CREATE inSensitive =
            {
                .size = 0,
                .sensitive = {
                    .userAuth = {
                        .size = (tpm_string_size)context.srkAuth.size(),
                        .buffer = {0},
                    },
                    .data = {.size = 0, .buffer = {0}}}};

        memcpy(inSensitive.sensitive.userAuth.buffer, context.srkAuth.c_str(), (tpm_string_size)context.srkAuth.size());
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

        TPM2B_DATA outsideInfo =
            {
                .size = 0,
                .buffer = {0},
            };

        TPML_PCR_SELECTION creationPCR =
            {
                .count = 0,
            };

        TPM2B_AUTH authValue =
            {
                .size = (tpm_string_size)context.ownerAuth.size(),
                .buffer = {0}};
        memcpy(authValue.buffer, context.ownerAuth.c_str(), (tpm_string_size)context.ownerAuth.size());

        try
        {
            response = Esys_StartAuthSession(esys_context, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, NULL, TPM2_SE_HMAC, &symmteric, TPM2_ALG_SHA256, &session);

            if (response != TPM2_SUCCESS)
            {
                throw std::invalid_argument("Failed to start auth session");
            }

            response = Esys_TR_SetAuth(esys_context, ESYS_TR_RH_OWNER, &authValue); // Authentication check for using owner hierarchy.

            if (response != TPM2_SUCCESS)
            {
                throw std::invalid_argument("Authentication failure");
            }

            response = Esys_CreatePrimary(esys_context, ESYS_TR_RH_OWNER, session,
                                          ESYS_TR_NONE, ESYS_TR_NONE, &inSensitive, &inPublic,
                                          &outsideInfo, &creationPCR, &objectHandle,
                                          &outPublic, &creationData, &creationHash,
                                          &creationTicket);

            if (response != TPM2_SUCCESS)
            {
                throw std::invalid_argument("Creating Primary key");
            }

            response = Esys_FlushContext(esys_context, session); // Flushing session

            if (response != TPM2_SUCCESS)
            {
                throw std::invalid_argument("ERROR during flush session context.");
            }

            response = createDek(esys_context, objectHandle, context);
            if (response != TPM2_SUCCESS)
            {
                throw std::invalid_argument("");
            }

            response = Esys_FlushContext(esys_context, objectHandle); // Flushing Object(stored key handle)

            if (response != TPM2_SUCCESS)
            {
                throw std::invalid_argument("during flush object context");
            }

            Esys_Free(outPublic);
            Esys_Free(creationData);
            Esys_Free(creationHash);
            Esys_Free(creationTicket);
        }
        catch (const std::exception &ex)
        {
            LOG_ERROR(ex.what());
            if ((objectHandle != ESYS_TR_NONE && Esys_FlushContext(esys_context, objectHandle)) != TPM2_SUCCESS)
            {
                LOG_ERROR("Clearing object handle failed...");
            }
            if ((session != ESYS_TR_NONE && Esys_FlushContext(esys_context, session)) != TPM2_SUCCESS)
            {
                LOG_ERROR("Failed to clear session context.");
            }
            Esys_Free(outPublic);
            Esys_Free(creationData);
            Esys_Free(creationHash);
            Esys_Free(creationTicket);
        }
        return response;
    }

    TPM2_RC createDek(ESYS_CONTEXT *esys_context, ESYS_TR srk_context, TpmContext &context)
    {
        ESYS_TR loadedKeyHandle = ESYS_TR_NONE;
        ESYS_TR persistent_handle = ESYS_TR_NONE;
        ESYS_TR session = ESYS_TR_NONE;
        TPMT_SYM_DEF symmteric = {.algorithm = TPM2_ALG_NULL};
        TPM2_RC response = TPM2_SUCCESS;
        TPM2B_PUBLIC *outPublic = NULL;
        TPM2B_PRIVATE *outPrivate = NULL;
        TPM2B_CREATION_DATA *creationData = NULL;
        TPM2B_DIGEST *creationHash = NULL;
        TPMT_TK_CREATION *creationTicket = NULL;
        TPM2B_SENSITIVE_DATA *outData = NULL;
        size_t buffer_size;
        uint8_t *buffer = NULL;
        uint8_t key_buffer[MAX_SEAL_BUFFER_SIZE];
        size_t key_buffe_size = MAX_SEAL_BUFFER_SIZE;

        std::strncpy(reinterpret_cast<char *>(key_buffer), context.data.c_str(), key_buffe_size - 1);
        key_buffer[key_buffe_size - 1] = 0;
        std::cout << "Copied string: " << reinterpret_cast<char *>(key_buffer) << '\n';

        DEBUG("Seal key size is : ", std::to_string(key_buffe_size));

        TPM2B_SENSITIVE_CREATE inSensitive =
            {
                .size = 0,
                .sensitive = {
                    .userAuth = {
                        .size = (tpm_string_size)context.dekAuth.size(),
                        .buffer = {0},
                    },
                    .data = {.size = (UINT16)key_buffe_size, .buffer = {0}}}};

        memcpy(inSensitive.sensitive.userAuth.buffer, context.dekAuth.c_str(), (tpm_string_size)context.dekAuth.size());
        memcpy(inSensitive.sensitive.data.buffer, key_buffer, key_buffe_size);

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
        inPublic.publicArea.unique.keyedHash.size = 0;
        memset(inPublic.publicArea.unique.keyedHash.buffer, 0, sizeof(inPublic.publicArea.unique.keyedHash.buffer));

        TPM2B_DATA outsideInfo =
            {
                .size = 0,
                .buffer = {0},
            };

        TPML_PCR_SELECTION creationPCR =
            {
                .count = 0,
            };

        TPM2B_AUTH primaryAuth =
            {
                .size = (tpm_string_size)context.srkAuth.size(),
                .buffer = {0},
            };

        memcpy(primaryAuth.buffer, context.srkAuth.c_str(), (tpm_string_size)context.srkAuth.size());
        try
        {
            response = Esys_StartAuthSession(esys_context, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, NULL, TPM2_SE_HMAC, &symmteric, TPM2_ALG_SHA256, &session);
            if (response != TPM2_SUCCESS)
            {
                throw std::invalid_argument("Failed to start auth session.");
            }

            response = Esys_Create(esys_context,
                                   srk_context,
                                   session, ESYS_TR_NONE, ESYS_TR_NONE,
                                   &inSensitive,
                                   &inPublic,
                                   &outsideInfo,
                                   &creationPCR,
                                   &outPrivate,
                                   &outPublic,
                                   &creationData, &creationHash, &creationTicket);

            if (response != TPM2_SUCCESS)
            {
                throw std::invalid_argument("Error creating DEK");
            }

            response = Esys_Load(esys_context,
                                 srk_context,
                                 session,
                                 ESYS_TR_NONE,
                                 ESYS_TR_NONE, outPrivate, outPublic, &loadedKeyHandle);

            if (response != TPM2_SUCCESS)
            {
                throw std::invalid_argument("Error load key into the TPM");
            }
            TPM2_HANDLE permanentHandle = TPM2_PERSISTENT_FIRST + context.index;
            response = Esys_EvictControl(esys_context, ESYS_TR_RH_OWNER, loadedKeyHandle,
                                         session, ESYS_TR_NONE, ESYS_TR_NONE,
                                         permanentHandle, &persistent_handle);

            if (response != TPM2_SUCCESS)
            {
                throw std::invalid_argument("Failed to persist the seal key.");
            }

            response = Esys_TR_Serialize(esys_context, persistent_handle, &buffer, &buffer_size);

            if (response != TPM2_SUCCESS)
            {
                throw std::invalid_argument("Error serializing DEK handle.");
            }

            if (write(context.fileName, buffer, buffer_size) != TPM2_SUCCESS)
            {
                throw std::invalid_argument("Unable to store the authorization context");
            }

            // response = updateNVIndex(context);

            if (response != TPM2_SUCCESS)
            {
                throw std::invalid_argument("Failed to add created handle to the database");
            }

            response = Esys_FlushContext(esys_context, session);

            if (response != TPM2_SUCCESS)
            {
                throw std::invalid_argument("Failed to flush session.");
            }

            // here if database failed to add the key we have to delete the seal key.

            Esys_Free(outPublic);
            Esys_Free(creationData);
            Esys_Free(creationHash);
            Esys_Free(creationTicket);
            Esys_Free(outData);
            INFO("DEK(Data Encryption Key) key created.");
        }
        catch (std::exception &ex)
        {
            if (session != ESYS_TR_NONE && Esys_FlushContext(esys_context, session) != TPM2_SUCCESS)
            {
                LOG_ERROR("Failed to clear session context.");
            }
            Esys_Free(outPublic);
            Esys_Free(creationData);
            Esys_Free(creationHash);
            Esys_Free(creationTicket);
            Esys_Free(outData);
            LOG_ERROR("DEK(Data Encryption Key) key creation error.");
        }
        return response;
    }

    int write(const string &filePath, uint8_t *data, size_t dataSize)
    {
        if (filePath.empty())
        {
            return SCM::FAILED;
        }
        fstream file(filePath, std::ios::out | std::ios::binary);
        if (file.is_open())
        {
            file.write(reinterpret_cast<char *>(data), dataSize);
            file.close();
        }
        return SCM::SUCCESS;
    }
};

#endif