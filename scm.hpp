// #include "test/os_test.hpp"
// #include "test/ConfigService_test.hpp"

// #include "test/Common_unit_test.hpp"
// #include "test/monitor_test.hpp"
// #include "test/ids_test.hpp"
// #include "test/log_unit_test.hpp"
// #include "rest/RestService.hpp"
// #include "tpm/tpm_service.hpp"

//  #include "test/mqtt_service_unit_test.hpp"
// #include "monitor/MonitorServiceImpl.hpp"

// #include "log_collector/LogController.hpp"

// #include "monitor/MonitorController.hpp"
// #include "util/logger.hpp"
// #include "ids/IdsController.hpp"
// #include "ids/IdsProxy.hpp"
// #include "rootkit/RootCheckService.hpp"
//  #include "test/patch_unit_test.hpp"

// #include "test/integration_test/log_test.hpp"
// #include "test/integration_test/monitor_test.hpp"
// #include "integration_test/patch_test.hpp"

#include "mqtt/MqttController.hpp"
#include "tpm/TpmService.hpp"

namespace TestMethods
{
    void startMqtt()
    {
        MqttController controller("/home/champ/scm/config/schedule.config");
        controller.start();
    }

    void testClearTpm()
    {
        TpmService service;
        TpmContext context;
        context.keyName = "seal1";
        context.ownerAuth = "";
        context.srkAuth = "";
        context.dekAuth = "dekAuth";
        context.data = "bcuwbeounpoifnfir3wnpofnrlhwg";
        std::cout << "tpm service instantiated\n";
        TPM2_RC response = service.seal_key(context);
        if (response == SCM::Tpm::TPM2_SUCCESS)
        {
            std::cout << "Tpm sealed successfully.\n";
        }
    }

    void trackMessageArrived()
    {
        config_table_type config_table;
        ConfigService config;
        int response = config.readIniConfigFile("/home/champ/scm/config/schedule.config", config_table);
        if (response != SCM::SUCCESS)
        {
            return;
        }
        Json::Value root;
        root["FromDate"] = "2024-05-29T19:33:40Z"; // May 29 19:33:40
        root["EndDate"] = "2024-05-29T17:43:19Z";  // May 29 17:43:19
        root["Id"] = "123e4567-e89b-12d3-a456-426614174000";
        root["SourceId"] = "source-456";
        root["TargetId"] = "target-123";
        root["ActionType"] = "LogRequest";
        root["logType"] = "syslog";
        root["IsAckRequired"] = true;
        root["ResponseType"] = "MqttResponse";
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, root);
        mqtt::const_message_ptr msg = mqtt::make_message("log", json_string);
        MqttClient::MqttCallback mqttCallback(config_table);
        mqttCallback.message_arrived(msg);
    }
}

// void validNotEMptyOrNull (const string &field , const &fieldName) 
// {
//     if (field.empty()) 
//     {
//         throw invalid_argument(filedName + " must not be emptyor null.");
    
//     }
// }