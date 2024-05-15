#include "scm.hpp"
#include "util/constants.hpp"

// void testMqtt()
// {

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