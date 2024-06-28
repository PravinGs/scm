#ifndef TPM_ENTITY_HPP
#define TPM_ENTITY_HPP
#include "util/Common.hpp"
#include "mqtt/MqttEntity.hpp"

// {
//     string keyName;
//     string ownerAuth;
//     string srkAuth;
//     string dekAuth;
//     string data;
//     string fileName;
//     int index;
//     int dataSize;       
//     int type;     
//     int command; 
   
//     TpmContext() : index(0), dataSize(0), type(-1), command(-1) {}
//     TpmContext(const TpmRequest& request) : 
//             keyName(request.keyName),
//             ownerAuth(request.ownerAuth), 
//             srkAuth(request.srkAuth),
//             dekAuth(request.dekAuth),
//             data(request.data),
//             fileName(request.fileName),
//             index(request.index),
//             dataSize(request.dataSize),
//             type(request.type){}

//     void setKeyName(string key) { this->keyName = key; }
//     void setDekAuth(string dek) { this->dekAuth = dek; }
// };

#endif