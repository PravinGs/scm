#include "scm.hpp"
#include "util/constants.hpp"

// void testMqtt()
// {

// }


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
//     root["FromDate"] = "2024-05-21T11:52:01Z"; //May 21 11:52:01
//     root["EndDate"] = "2024-05-20T23:59:59Z";
//     root["Id"] = "123e4567-e89b-12d3-a456-426614174000";
//     root["SourceId"] = "source-456";
//     root["TargetId"] = "target-123";
//     root["ActionType"] = "LogRequest";
//     root["logType"] = "syslog";
//     root["IsAckRequired"] = true;
//     root["ResponseType"] = "MqttResponse";
//     Json::StreamWriterBuilder builder;
//     std::string json_string = Json::writeString(builder, root);
//     mqtt::const_message_ptr msg = mqtt::make_message("log", json_string);
//     MqttClient::MqttCallback mqttCallback(config_table);
//     mqttCallback.message_arrived(msg);
// }

// int main(int argc, char **argv)
// {
//     Common::parseArguments(argc, argv);
//     openlog("agent.service", LOG_INFO | LOG_CONS, LOG_USER);
//     SCM::is_syslog_enabled = true;
//     Common::setupLogger(SCM::Config::LOG_PATH);
//     if (!SCM::is_syslog_enabled)
//     {
//         Common::logfp.open(SCM::Config::LOG_PATH, std::ios::app);
//     }
//     trackMessageArrived();
//     // MqttController controller("/home/champ/scm/config/schedule.config");
//     // controller.start();
//     if (Common::logfp.is_open())
//     {
//         Common::logfp.close();
//     }
//     closelog();
// }

int main(int argc, char **argv)
{
    // std::cout << "build completed\n";
    Common::parseArguments(argc, argv);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}