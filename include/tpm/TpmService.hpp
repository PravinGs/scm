#ifndef TPM_SERVICE_HPP
#define TPM_SERVICE_HPP

extern "C"
{
#include <tpm/adminservice.h>
#include <tpm/dbservice.h>
#include <tpm/keyservice.h>
#include <tpm/sealservice.h>
#include <tpm/encryptservice.h>
#include <tpm/tpm_common.h>
}
#include "util/Common.hpp"
#include "util/constants.hpp"
class TpmService
{
private:
    ESYS_CONTEXT *g_esys_context = nullptr;
    TSS2_TCTI_CONTEXT *g_tcti_context = nullptr;
    bool g_context_state = true;
    const char *DEVICE_ADDRESS = "/dev/tpm0";
    size_t device_size = 0;

    void setup()
    {
        // TPM2_RC  response = Tss2_TctiLdr_Initialize(SIMULATOR_ADDRESS, &g_tcti_context);
        TPM2_RC response = Tss2_Tcti_Device_Init(g_tcti_context, &device_size, DEVICE_ADDRESS);
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

    bool check_tpm2_device(string &err_msg)
    {
        bool status = std::system("dmesg | grep -i \"tpm2.0\"") == 0 ? true : false;
        if (!status)
        {
            err_msg += "TPM device not exist\n";
            return SCM::Tpm::TPM2_DEVICE_NOT_EXIST;
        }
        return status;
    }

    bool check_tss2_stack(string &err_msg)
    {
        bool local_status = std::system("sudo ls /usr/local/bin | grep -i \"tss2\"") == 0 ? true: false;
        bool status = std::system("sudo ls /usr/bin | grep -i \"tss2\"") == 0 ? true:false;
        if ((!local_status) && (!status))
        {
            err_msg += "TSS2 Stack Library not exist\n";
            return SCM::Tpm::TSS2_LIBRARY_NOT_EXIST;
        }
        return  local_status || status; 
    }

    int check_tpm2_abrmd_service(string &err_msg)
    {
        int status = std::system("systemctl is-active --quiet tpm2-abrmd");
        if (status != 0)
        {
            err_msg += "TPM2-abrmd service is not active, starting tpm2-abrmd service\n";
            status = std::system("systemctl start --quiet tpm2-abrmd");
        }

        if (status == 0)
        {
            status = std::system("libtss2-tcti-mssim.so.0:host=127.0.0.1,port=2321");
            return (status == 0) ? SCM::Tpm::TPM2_ABRMD_SIMULATOR_ACTIVE : SCM::Tpm::TPM2_ABRMD_DEVICE_ACTIVE;
        }
        else
        {
            err_msg += "TPM2-abrmd service failed to start\n";
        }

        return SCM::Tpm::TPM2_ABRMD_NOT_ACTIVE;
    }

public:
    TpmService()
    {
        setup();
    }

    TPM2_RC clear_tpm(const char *lockout_auth)
    {
        return clearTpm(g_esys_context, lockout_auth);
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

    int nv_read(PERSISTED_CONTEXT &context, char *key)
    {
        char data[2048];
        return nvRead(g_esys_context, context, data);
    }

    int testTpmConnection(string &err_msg)
    {
        if ((!check_tpm2_device(err_msg)) && (!check_tss2_stack(err_msg)) && (check_tpm2_abrmd_service(err_msg) == SCM::Tpm::TPM2_ABRMD_NOT_ACTIVE))
        {
            return SCM::FAILED;
        }
        return SCM::Tpm::TPM2_TEST_SUCCESS;
    }

    ~TpmService()
    {
        Tss2_Tcti_Finalize(g_tcti_context);
        Esys_Free(g_esys_context);
        g_tcti_context = NULL;
        g_esys_context = NULL;
    }
};

#endif
