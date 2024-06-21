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
    int dataSize;       
    int type;      
   
    TpmContext() : index(0), dataSize(0), type(-1) {}

    void setKeyName(string key) { this->keyName = key; }
    void setDekAuth(string dek) { this->dekAuth = dek; }
};

#endif