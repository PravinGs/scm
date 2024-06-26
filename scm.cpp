#include "scm.hpp"
#include "util/constants.hpp"
#include "util/ConfigService.hpp"
#include "util/EntityParser.hpp"

using namespace TestMethods;

int main(int argc, char **argv)
{
    Common::parseArguments(argc, argv);
    openlog("agent.service", LOG_INFO | LOG_CONS, LOG_USER);
    SCM::is_syslog_enabled = true;
    Common::setupLogger(SCM::Config::LOG_PATH);
    if (!SCM::is_syslog_enabled)
    {
        Common::logfp.open(SCM::Config::LOG_PATH, std::ios::app);
    }

    startMqtt();

    if (Common::logfp.is_open())
    {
        Common::logfp.close();
    }
    closelog();
}

// int main(int argc, char **argv)
// {
//     // std::cout << "build completed\n";
//     Common::parseArguments(argc, argv);
//     testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }

// int main()
// {
//     PatchServiceImpl Patch;
//     PatchEntity& entity;
//     Patch.isFileComplete(entity);
//     return 0;
// }

// string validatePatchRequest (const string &patchversion,  const string &url, const string &username , const string &password)
// {
//     validateNotEmptyOrNull (patchversion , "patch_version" );
//     validateNotEmptyOrNull ( url,"url" );
//     validateNotEmptyOrNull ( username, "username" );
//     validateNotEmptyOrNull ( password, "Password ");
// }
