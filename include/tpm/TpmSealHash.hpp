#ifndef TPM_SEAL_HPP
#define TPM_SEAL_HPP
#include "TpmEntity.hpp"
#include "KeyHandler.hpp"

using namespace SCM::Tpm;
class TpmSeal
{
public:
    TPM2_RC add(ESYS_CONTEXT *esys_context, TpmContext &context)
    {
        if (esys_context == NULL)
        {
            LOG_ERROR("TPM2_INVALID_ESYS_CONTEXT");
            return TPM2_INVALID_ESYS_CONTEXT;
        }

        TPM2_RC result = validateTpmContext(context);
        if (result != TPM2_SUCCESS)
        {
            return result;
        }
        // int response = TPM2_INVALID_OBJECT_CREDENTIAL;
        int response = TPM2_SUCCESS;
        int sealIndex = TPM2_PERSISTENT_FIRST;
        int index = 1;
        // response = isNameExist(context.keyName);
        // if (response == TPM2_INVALID_DATABASE)
        // {
        //     return response;
        // }
        // else if (response == TPM2_DUPLICATE_VALUE)
        // {
        //     LOG_ERROR("Key alread store in tpm with this name", context.keyName);
        //     return response;
        // }
        // while (index < 3)
        // {
        //     response = existByIndex(sealIndex + index);
        //     if (response == TPM2_SUCCESS)
        //     {
        //         break;
        //     }
        //     index++;
        // }

        if (response == TPM2_SUCCESS && index >= 3)
        {
            return response;
        }
        else
        {
            sealIndex = sealIndex + index;
            context.index = index;
            context.fileName = std::to_string(sealIndex);
            DEBUG("The created Path : " + std::to_string(sealIndex));
            return addHash(esys_context, context);
        }
    }

private:
    TPM2_RC addHash(ESYS_CONTEXT *esys_context, TpmContext& context)
    {
        TPM2_RC response = TPM2_SUCCESS;
        // TPM2_HANDLE permanentHandle = TPM2_PERSISTENT_FIRST + index;
        try
        {
            response = key.createSrk(esys_context, context);

            if (response != TPM2_SUCCESS)
            {
                throw std::invalid_argument("Primary key creation failed.");
            }
        }
        catch (std::exception &ex)
        {
            LOG_ERROR("Failed to seal secret into TPM, " , ex.what());
        }

        return response;
    }

    TPM2_RC validateTpmContext(const TpmContext &context)
    {
        if (context.dekAuth.empty() || context.data.empty() || context.keyName.empty())
        {
            DEBUG("TPM2_EMPTY_STRING");
            return TPM2_EMPTY_STRING;
        }

        return TPM2_SUCCESS;
    }

private:
    KeyHandler key;

};

#endif