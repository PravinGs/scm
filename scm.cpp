#include "scm.hpp"
#include "util/constants.hpp"
#include "util/ConfigService.hpp"
#include "util/entity_parser.hpp"
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
//     root["FromDate"] = "2024-05-29T19:33:40Z"; //May 29 19:33:40
//     root["EndDate"] = "2024-05-29T17:43:19Z"; //May 29 17:43:19
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
    // trackMessageArrived();
    
    // if (Common::stringToTime("2024-06-03 12:17:01") > Common::utcStringToTime("2024-06-03T12:17:00Z"))
    // {
    //     std::cout << "Both ok\n";
    // }
//     MqttController controller("/home/champ/scm/config/schedule.config");
//     controller.start();
//     if (Common::logfp.is_open())
//     {
//         Common::logfp.close();
//     }
//     closelog();
// }

// int main(int argc, char **argv)
// {
//     // std::cout << "build completed\n";
//     Common::parseArguments(argc, argv);
//     testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }

// int main(int argc, char** argv)
// {
//     Common::parseArguments(argc, argv);
//     ConfigService config;
//     config_table_type config_table;
//     int response = config.readIniConfigFile("/home/champ/scm/config/schedule.config", config_table);
//     if (response != SCM::SUCCESS)
//     {
//         return -1;
//     }
//     std::string zipFilePath = "/var/log/syslog.2.gz";
//     // std::string fileNameInZip = "syslog.2";

//     // read_gz_file(zipFilePath);

//     LogServiceImpl logCheck;
    
//     entity_parser parser;
//     LogEntity entity = parser.getLogEntity(config_table, "syslog");
//     entity.last_read_time = Common::stringToTime("2024-05-29 11:40:00"); 
//     vector<string> logs;
//     std::time_t end_time = Common::stringToTime("2024-05-29 11:50:00");
//     int counter = 0;
//     logCheck.readSysLogFile(entity, logs, end_time, counter);
//     std::cout<< logs.size() << std::endl;
//     if (logs.size() > 0)
//     {
//         for (size_t i = 0; i < logs.size(); i++)
//         {
//             std::cout << logs[i] << '\n';
//         }
//     }
//     return 0;
// }

    int main()
    {
        MonitorServiceImpl monitor;
        const ProcessEntity entity;
        vector<ProcessData> logs;
        monitor.getAppResourceDetailsName(entity, logs);
        std::cout << logs.size() << std::endl;
        return 0;
    }