#ifndef TPM_EXCEPTION_HPP
#define TPM_EXCEPTION_HPP

#define CONVERT_UNSIGNED_TO_SIGNED(value) ((int)(value))
#pragma once
#include "util/Common.hpp"
#include <tss2/tss2_rc.h>
using namespace SCM::Tpm;
class TPMException : public std::runtime_error
{
public:
    TPMException(const std::string &message, TPM2_RC code)
        : std::runtime_error(message), errorCode(code) {}
    TPM2_RC errorCode;
};

static void handleTPMResponse(TPM2_RC response, const std::string &errorMessage)
{
    if (response != TPM2_RC_SUCCESS)
    {
        string logLevelString;
        switch (CONVERT_UNSIGNED_TO_SIGNED(response))
        {
        case TPM2_INVALID_HIERARCHY_AUTH:
            logLevelString = "INVALID AUTH VALUE";
            break;
        case TPM2_INVALID_ESYS_CONTEXT:
            logLevelString = "INVALID ESYS CONTEXT";
            break;
        case TPM2_EXCEEDED_MAX_SEAL_BUFFER_SIZE:
            logLevelString = "KEY SIZE SHOULD NOT BE EXCEED 256";
            break;
        case TPM2_EMPTY_KEY_FILE:
            logLevelString = "KEY SIZE SHOULD NOT BE 0";
            break;
        case TPM2_INVALID_OBJECT:
            logLevelString = "INVALID OBJECT";
            break;
        case TPM2_DUPLICATE_VALUE:
            logLevelString = "DUPLICATE VALUE";
            break;
        case TPM2_GENERIC_MESSAGE:
            logLevelString = "GENERIC";
            break;
        case TPM2_INVALID_DEVICE:
            logLevelString = "INVALID DEVICE";
            break;
        case TPM2_INVALID_FILE:
            logLevelString = "INVALID FILE";
            break;
        default:
            logLevelString = "UNKNOWN";
            break;
        }
        const char *error = Tss2_RC_Decode(response);
        std::cout << error << "\t Response code: " << response << '\n';
        throw TPMException(logLevelString + errorMessage, response);
    }
}
#endif