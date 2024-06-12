#include "MqttTpmHandler.hpp"

template <>
int MqttTpmHandler::handle(BaseRequest &request)
{
    return testTpmConfiguration();
}

template <>
int MqttTpmHandler::handle(TpmClearRequest &request)
{
    int result = tpm->clear_tpm(request.lockoutAuth.c_str());

    return (result == TPM2_SUCCESS) ? SCM::SUCCESS : result;
}

int MqttTpmHandler::testTpmConfiguration()
{
    int result = SCM::SUCCESS;
    if ((result = isTpm2Exist()) != SCM::SUCCESS)
    {
        return result;
    }
    result = isAbrmdServiceRunning();
    if (result != SCM::Tpm::TPM2_ABRMD_DEVICE_ACTIVE && result != SCM::Tpm::TPM2_ABRMD_SIMULATOR_ACTIVE)
    {
        return result;
    }
    result = isTss2LibraryInstalled();
    return result;
}

int MqttTpmHandler::isTpm2Exist()
{
    bool status = std::system("dmesg | grep -i \"tpm2.0\"") == 0 ? true : false;
    if (!status)
    {
        return SCM::Tpm::TPM2_DEVICE_NOT_EXIST;
    }
    return status ? SCM::SUCCESS : SCM::Tpm::TPM2_DEVICE_NOT_EXIST;
}

int MqttTpmHandler::isTss2LibraryInstalled()
{
    int status = std::system("ls /usr/local/bin | grep -i tss2 --quiet");
    bool localLibStatus = (status == 0) ? true : false;
    status = -1;
    status = std::system("ls /usr/bin | grep -i tss2 --quiet");
    bool libStatus = (status == 0) ? true : false;
    return (localLibStatus || libStatus) ? SCM::SUCCESS : SCM::Tpm::TSS2_LIBRARY_NOT_EXIST;
}

int MqttTpmHandler::isAbrmdServiceRunning()
{
    int status = std::system("systemctl is-active --quiet tpm2-abrmd");
    if (status != 0)
    {
        status = std::system("systemctl start --quiet tpm2-abrmd");
    }

    if (status == 0)
    {
        status = std::system("systemctl status tpm2-abrmd.service | grep -i /dev/tpm0");
        return (status == 0) ? SCM::Tpm::TPM2_ABRMD_DEVICE_ACTIVE : SCM::Tpm::TPM2_ABRMD_SIMULATOR_ACTIVE;
    }
    else
    {
    }

    return SCM::Tpm::TPM2_ABRMD_NOT_ACTIVE;
}