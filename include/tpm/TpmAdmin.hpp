#ifndef TPM_ADMIN_HPP
#define TPM_ADMIN_HPP

#include "util/Common.hpp"
#include "db/DbService.hpp"
#include "TpmException.hpp"
using namespace SCM::Tpm;
class TpmAdmin
{
public:
    TpmAdmin() : db(std::make_unique<DbService>(SCM::Tpm::DEFAULT_DB_PATH))
    {
    }

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
            handleTPMResponse(response, "failed to start auth session.");
            
            response = Esys_TR_SetAuth(esys_context, typeHandle, &oldPasword);
            handleTPMResponse(response, "failed to set auth value.");
            
            response = Esys_HierarchyChangeAuth(esys_context,
                                                typeHandle,
                                                session,
                                                ESYS_TR_NONE,
                                                ESYS_TR_NONE,
                                                &newPassword);
            handleTPMResponse(response, "error at changing the password..");
            
            response = Esys_FlushContext(esys_context, session);
            handleTPMResponse(response, "failed to flush the session context.");
            return response;
        }
        catch (TPMException &ex)
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
            handleTPMResponse(response, "Failed to start auth session");
            response = Esys_TR_SetAuth(esys_context, ESYS_TR_RH_LOCKOUT, &authValue); // Authentication check for using owner hierarchy.
            handleTPMResponse(response, "Failed to set auth value");

            response = Esys_Clear(esys_context, ESYS_TR_RH_LOCKOUT, session, ESYS_TR_NONE, ESYS_TR_NONE);
            handleTPMResponse(response, "Error during TPM Clear...");

            response = Esys_FlushContext(esys_context, session);

            handleTPMResponse(response, "Failed to flush session.");
            response = clearHandles(); // Requires DB connection
            return response;
        }
        catch (const TPMException &e)
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
    int clearHandles()
    {
        string command = "rm -rf " + SCM::Tpm::TPM_AUTHORIZATION_STORAGE;
        if (std::system(command.c_str()) == 0)
        {
            std::cout << "Files removed from TPM_AUTHORIZATION_STORAGE\n";
        }
        return db->deleteTpmContextAllHandles();
    }

private:
    std::unique_ptr<DbService> db;
};
#endif