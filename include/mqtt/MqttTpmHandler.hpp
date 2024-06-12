#ifndef MQTT_TPM_HANDLER_HPP
#define MQTT_TPM_HANDLER_HPP

#include "tpm/TpmService.hpp"
class MqttTpmHandler
{
public:
    MqttTpmHandler() : tpm(std::make_unique<TpmService>()) {}
    template <typename T>
    int handle(T &request);

private:
    int testTpmConfiguration();
    int isTpm2Exist();
    int isTss2LibraryInstalled();
    int isAbrmdServiceRunning();

private:
    std::unique_ptr<TpmService> tpm;
};

#endif