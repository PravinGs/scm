#ifndef TPM_ADMIN_HPP
#define TPM_ADMIN_HPP

#include "util/Common.hpp"
using namespace SCM::Tpm;
class TpmAdmin
{
public:

    TPM2_RC setAuthHierarchy(ESYS_CONTEXT *esys_context, const int type, const string &oldAuth, const string &newAuth)
    {
        if (esys_context == nullptr)
        {
            LOG_ERROR("TPM2_INVALID_ESYS_CONTEXT");
            return TPM2_INVALID_ESYS_CONTEXT;
        }
        TPM2_RC response = TPM2_SUCCESS;
        ESYS_TR typeHandle = ESYS_TR_NONE;
        ESYS_TR session = ESYS_TR_NONE;
        TPMT_SYM_DEF symmteric = {.algorithm = TPM2_ALG_NULL};
        switch (type)
        {
        case 1:
            typeHandle = ESYS_TR_RH_OWNER;
            break;
        case 2:
            typeHandle = ESYS_TR_RH_PLATFORM;
            break;
        case 3:
            typeHandle = ESYS_TR_RH_ENDORSEMENT;
            break;
        case 4:
            typeHandle = ESYS_TR_RH_LOCKOUT;
            break;
        default:
            typeHandle = ESYS_TR_RH_OWNER;
        }

        TPM2B_AUTH oldPasword =
            {
                .size = (tpm_string_size)oldAuth.size(),
                .buffer = {0}};

        memcpy(oldPasword.buffer, oldAuth.c_str(), (tpm_string_size)oldAuth.size());

        TPM2B_AUTH newPassword =
            {
                .size = (tpm_string_size)newAuth.size(),
                .buffer = {0}};

        memcpy(newPassword.buffer, newAuth.c_str(), (tpm_string_size)newAuth.size());
        try
        {
            response = Esys_StartAuthSession(esys_context, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, NULL, TPM2_SE_HMAC, &symmteric, TPM2_ALG_SHA256, &session);
            if (response != TPM2_SUCCESS)
            {
                throw std::invalid_argument("failed to start auth session.");
            }
            response = Esys_TR_SetAuth(esys_context, typeHandle, &oldPasword);
            if (response != TPM2_SUCCESS)
            {
                throw std::invalid_argument("failed to set auth value.");
            }
            response = Esys_HierarchyChangeAuth(esys_context,
                                                typeHandle,
                                                session,
                                                ESYS_TR_NONE,
                                                ESYS_TR_NONE,
                                                &newPassword);
            if (response != TPM2_SUCCESS)
            {
                throw std::invalid_argument("error at changing the password..");
            }
            response = Esys_FlushContext(esys_context, session);

            if (response != TPM2_SUCCESS)
            {
                throw std::invalid_argument("failed to flush the session context.");
            }

            return response;
        }
        catch (std::exception &ex)
        {
            LOG_ERROR(ex.what());
            if (session != ESYS_TR_NONE && Esys_FlushContext(esys_context, session) != TPM2_SUCCESS)
            {
                LOG_ERROR("failed to clear session context.");
            }
            return response;
        }
    }

    TPM2_RC clearTpm(ESYS_CONTEXT *esys_context, const string &hierarchyAuth)
    {
        TPM2_RC response = TPM2_SUCCESS;
        ESYS_TR session = ESYS_TR_NONE;
        TPMT_SYM_DEF symmteric = {.algorithm = TPM2_ALG_NULL};

        TPM2B_AUTH authValue =
            {
                .size = static_cast<tpm_string_size>(hierarchyAuth.size()),
                .buffer = {0}};

        memcpy(authValue.buffer, hierarchyAuth.c_str(), (tpm_string_size)hierarchyAuth.size());
        try
        {
            if (esys_context == nullptr)
            {
                LOG_ERROR("TPM2_INVALID_ESYS_CONTEXT");
                return TPM2_INVALID_ESYS_CONTEXT;
            }

            response = Esys_StartAuthSession(esys_context, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, NULL, TPM2_SE_HMAC, &symmteric, TPM2_ALG_SHA256, &session);
            if (response != TPM2_SUCCESS)
            {

                throw std::invalid_argument("Failed to start auth session");
            }
            response = Esys_TR_SetAuth(esys_context, ESYS_TR_RH_LOCKOUT, &authValue); // Authentication check for using owner hierarchy.
            if (response != TPM2_SUCCESS)
            {
                throw std::invalid_argument("Failed to set auth value");
            }

            response = Esys_Clear(esys_context, ESYS_TR_RH_LOCKOUT, session, ESYS_TR_NONE, ESYS_TR_NONE);

            if (response != TPM2_SUCCESS)
            {
                throw std::invalid_argument("Error during TPM Clear...");
            }

            response = Esys_FlushContext(esys_context, session);

            if (response != TPM2_SUCCESS)
            {
                throw std::invalid_argument("Failed to flush session.");
            }
            // response = clearHandles(); // Requires DB connection
            return response;
        }
        catch (const std::exception &e)
        {
            LOG_ERROR(e.what());
            if (session != ESYS_TR_NONE && Esys_FlushContext(esys_context, session) != TPM2_SUCCESS)
            {
                LOG_ERROR("Failed to clear session context.");
            }
        }
        return response;
    }

private:
};

#endif