#ifndef TPM_SERVICE_HPP
#define TPM_SERVICE_HPP
 
extern "C"
{
#include <tpm/adminservice.h>
#include <tpm/dbservice.h>
#include <tpm/keyservice.h>
#include <tpm/sealservice.h>
#include <tpm/encryptservice.h>
}
#include "util/common.hpp"
class tpm
{
private:
    ESYS_CONTEXT *g_esys_context = nullptr;
    TSS2_TCTI_CONTEXT *g_tcti_context = nullptr;
    bool g_context_state = true;
 
    void setup()
    {
        TPM2_RC response = Tss2_TctiLdr_Initialize(SIMULATOR_ADDRESS, &g_tcti_context);
        if (response != TPM2_SUCCESS)
        {
            g_context_state = false;
        }
        response = Esys_Initialize(&g_esys_context, g_tcti_context, NULL);
        if (response != TPM2_SUCCESS)
        {
            g_context_state = false;
        }
        g_context_state = (initialize_db(DEFAULT_DB_PATH) == TPM2_INVALID_DATABASE) ? true : false;
    }
 
public:
    tpm()
    {
        setup();
    }
 
    int clear_tpm(const char *lockout_auth)
    {
        return (int)clearTpm(g_esys_context, lockout_auth);
    }
 
    int seal_key(SEAL_CONTEXT &seal)
    {
        return sealProxy(g_esys_context, seal);
    }
 
    int unseal_key(SEAL_CONTEXT &seal)
    {
        return unSealSecret(g_esys_context, seal);
    }
 
    int nv_write(PERSISTED_CONTEXT &context)
    {
        return nvProxy(g_esys_context, context);
    }
 
    int nv_read(PERSISTED_CONTEXT &context, char* key)
    {
        char data[2048];
        return nvRead(g_esys_context, context, data);
    }
 
    ~tpm()
    {
        Tss2_Tcti_Finalize(g_tcti_context);
        Esys_Free(g_esys_context);
        g_tcti_context = NULL;
        g_esys_context = NULL;
    }
};
 
#endif