#ifndef TPM_SERVICE_HPP
#define TPM_SERVICE_HPP

// extern "C"
// {
// #include <tpm/adminservice.h>
// #include <tpm/dbservice.h>
// #include <tpm/keyservice.h>
// #include <tpm/sealservice.h>
// #include <tpm/encryptservice.h>
// #include <tpm/tpm_common.h>
// }
#include "util/Common.hpp"
#include "util/constants.hpp"
#include "mqtt/MqttEntity.hpp"
#include "db/DbService.hpp"
#include "TpmEntity.hpp"
#include "TpmAdmin.hpp"
#include "TpmSealHash.hpp"
#include "TpmNv.hpp"
class TpmService
{
    void setup()
    {
        // TPM2_RC  response = Tss2_TctiLdr_Initialize(SIMULATOR_ADDRESS, &g_tcti_context);
        TPM2_RC response = Tss2_Tcti_Device_Init(g_tcti_context, &device_size, DEVICE_ADDRESS);
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

public:
    TpmService() : db(std::make_unique<DbService>(TPM_CONTEXT_DB_PATH))
    {
        setup();
    }
    TPM2_RC clear_tpm(const string &lockoutAuth)
    {
        TPM2_RC response = admin.clearTpm(g_esys_context, lockoutAuth);
        return (response == SCM::Tpm::TPM2_SUCCESS) ? db->deleteTpmContextAllHandles() : response;
    }

    TPM2_RC changePassword(const int type, const string &oldAuth, const string &newAuth)
    {
        return admin.setAuthHierarchy(g_esys_context, type, oldAuth, newAuth);
    }

    int seal_key(TpmContext &context)
    {
        return seal.add(g_esys_context, context);
    }

    int unseal_key(TpmContext &context)
    {
        return seal.unSealSecret(g_esys_context, context);
    }

    int nv_write(TpmContext &context)
    {
        return nv.nvProxy(g_esys_context, context);
    }

    int nv_read(TpmContext &context)
    {
        return nv.nvRead(g_esys_context, context);
    }

    int nv_delete(TpmContext& context)
    {
        return nv.nvDelete(g_esys_context, context);
    }

    ~TpmService()
    {
        Tss2_Tcti_Finalize(g_tcti_context);
        Esys_Free(g_esys_context);
        g_tcti_context = NULL;
        g_esys_context = NULL;
    }

private:
    std::unique_ptr<DbService> db;
    ESYS_CONTEXT *g_esys_context = nullptr;
    TSS2_TCTI_CONTEXT *g_tcti_context = nullptr;
    bool g_context_state = true;
    const char *DEVICE_ADDRESS = "/dev/tpm0";
    size_t device_size = 0;
    TpmAdmin admin;
    TpmSeal seal;
    TpmNv nv;
};

#endif
