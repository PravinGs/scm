#ifndef TPM_ENTITY_HPP
#define TPM_ENTITY_HPP
#include "util/Common.hpp"

struct TpmContext
{
    string keyName;
    string ownerAuth;
    string srkAuth;
    string dekAuth;
    string data;
    string fileName;
    int index;

    TpmContext(): index(0){}
};

#endif