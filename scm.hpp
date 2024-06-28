// #include "test/os_test.hpp"
// #include "test/ConfigService_test.hpp"
// #include "test/Common_unit_test.hpp"
// #include "test/monitor_test.hpp"
// #include "test/ids_test.hpp"
// #include "test/log_unit_test.hpp"
// #include "test/patch_unit_test.hpp"
// #include "test/mqtt_service_unit_test.hpp"
// #include "test/tpm_test/admin_test.hpp"
// #include "test/tpm_test/tpmSeal_test.hpp"
// #include "test/tpm_test/tpmNv_test.hpp"
// #include "test/dbService_test.hpp"


// #include "test/integration_test/log_test.hpp"
// #include "test/integration_test/monitor_test.hpp"
// #include "integration_test/patch_test.hpp"


// #include "rest/RestService.hpp"
// #include "tpm/tpm_service.hpp"

// #include "monitor/MonitorServiceImpl.hpp"

// #include "log_collector/LogController.hpp"

// #include "monitor/MonitorController.hpp"
// #include "util/logger.hpp"
// #include "ids/IdsController.hpp"
// #include "ids/IdsProxy.hpp"
// #include "rootkit/RootCheckService.hpp"

// #include "patch/PatchServiceImpl.hpp"
 #include "mqtt/MqttController.hpp"
#include "tpm/TpmService.hpp"
// #include "db/DbService.hpp"

namespace TestMethods
{
    void startMqtt()
    {
        MqttController controller("/home/champ/scm/config/schedule.config");
        controller.start();
    }

    void unsealHash()
    {
        TpmService service;
        TpmContext context;
        const string hash = "bcuwbeounpoifnfir3wnpofnrlhwg";
        context.keyName = "seal1";
        context.ownerAuth = "";
        context.dekAuth = "dekAuth";
        TPM2_RC response = service.unseal_key(context);
        if (response == SCM::Tpm::TPM2_SUCCESS)
        {
            std::cout << context.data << '\n';
        }
    }

    void nvDelete()
    {
        TpmService service;
        TpmContext context;
        context.keyName = "nv1";
        context.ownerAuth = "";
        context.dekAuth = "dekAuth";
        TPM2_RC response = service.nv_delete(context);
        if (response == SCM::Tpm::TPM2_SUCCESS)
        {
            std::cout << "NV space cleared\n";
        }
        else{
            std::cerr << "Failed to undefine nv ram\n";
        }
    }

    void nvRead()
    {
        TpmService service;
        TpmContext context;
        context.keyName = "nv1";
        context.ownerAuth = "";
        context.dekAuth = "dekAuth";
        TPM2_RC response = service.nv_read(context);
        if (response == SCM::Tpm::TPM2_SUCCESS)
        {
            std::cout << context.data << '\n';
        }
    }

    void testSealTpm()
    {
        TpmService service;
        service.clear_tpm("");
        TpmContext context;
        const string hash = "bcuwbeounpoifnfir3wnpofnrlhwg";
        context.dataSize = hash.size();
        context.keyName = "seal1";
        context.ownerAuth = "";
        context.srkAuth = "";
        context.dekAuth = "dekAuth";
        context.data = hash;
        context.type = 0;
        std::cout << "tpm service instantiated\n";
        TPM2_RC response = service.seal_key(context);
        if (response == SCM::Tpm::TPM2_SUCCESS)
        {
            std::cout << "Tpm sealed successfully.\n";
            context.data = "";
            std::cout << "Before Tpm seal: " << context.data << '\n';
            context.dataSize = 0;
            response = service.unseal_key(context);
            if (response == SCM::Tpm::TPM2_SUCCESS)
            {
                std::cout << context.data << '\n';
            }
        }
    }

    void testNV()
    {
        TpmService service;
        service.clear_tpm("");
        TpmContext context;
        const string hash = "bcuwbeounpoifnfir3wnpofnrlhwg";
        context.dataSize = hash.size();
        context.keyName = "nv1";
        context.ownerAuth = "";
        context.srkAuth = "";
        context.dekAuth = "dekAuth";
        context.data = hash;
        context.type = 1;
        std::cout << "tpm service instantiated\n";
        TPM2_RC response = service.nv_write(context);
        if (response == SCM::Tpm::TPM2_SUCCESS)
        {
            std::cout << "Tpm sealed successfully.\n";
            context.data = "";
            std::cout << "Before Tpm seal: " << context.data << '\n';
            context.dataSize = 0;
            response = service.nv_read(context);
            if (response == SCM::Tpm::TPM2_SUCCESS)
            {
                std::cout << context.data << '\n';
            }
        }
    }

    // void trackMessageArrived()
    // {
    //     config_table_type config_table;
    //     ConfigService config;
    //     int response = config.readIniConfigFile("/home/champ/scm/config/schedule.config", config_table);
    //     if (response != SCM::SUCCESS)
    //     {
    //         return;
    //     }
    //     Json::Value root;
    //     root["FromDate"] = "2024-05-29T19:33:40Z"; // May 29 19:33:40
    //     root["EndDate"] = "2024-05-29T17:43:19Z";  // May 29 17:43:19
    //     root["Id"] = "123e4567-e89b-12d3-a456-426614174000";
    //     root["Topic"] = "source-456";
    //     root["TargetId"] = "target-123";
    //     root["ActionType"] = "LogRequest";
    //     root["logType"] = "syslog";
    //     root["IsAckRequired"] = true;
    //     root["ResponseType"] = "Mqtt";
    //     Json::StreamWriterBuilder builder;
    //     std::string json_string = Json::writeString(builder, root);
    //     mqtt::const_message_ptr msg = mqtt::make_message("log", json_string);
    //     MqttClient::MqttCallback mqttCallback(config_table);
    //     mqttCallback.message_arrived(msg);
    // }

    void testDatabase()
    {
        DbService db("/home/champ/TPM/Tpm-Server/src/secrets");
        TpmContext context;
        context.keyName = "";
        context.dekAuth = "Password";
        context.index; 
        context.dataSize;
        int result = db.addTpmContext(context);
        if (result == SCM::SUCCESS)
        {
            std::cout << "TpmContext Created.\n";
        }
    }
}






// // void validNotEMptyOrNull (const string &field , const &fieldName)
// // {
// //     if (field.empty())
// //     {
// //         throw invalid_argument(filedName + " must not be emptyor null.");

// //     }
// // }