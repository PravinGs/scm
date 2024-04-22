#include "scm.hpp"
#include "util/constants.hpp"

int debug_enabled = 1;
int console_log_enabled = 1;

void enable_console_log()
{
    console_log_enabled = 1;
}

void enable_debug() 
{
    debug_enabled = 1;
}

// std::mutex logs_mutex;

void test_log_controller()
{
    log_controller controller("/home/champ/scm/config/schedule.config");
    controller.start();
}

void test_process_controller()
{
    monitor_controller controller("/home/champ/scm/config/schedule.config");
    controller.start();
}

// void test_ids_controller()
// {
//     ids_controller controller("/home/champ/scm/config/schedule.config");
//     controller.start();
// }

int main(int argc, char **argv)
{
    openlog("agent.service", LOG_INFO | LOG_CONS, LOG_USER);
    Audit::is_syslog_enabled = true;
    common::setup_logger(Audit::Config::LOG_PATH);
    if (!Audit::is_syslog_enabled)
    {
        common::logfp.open(Audit::Config::LOG_PATH, std::ios::app);
    }
    // enable_debug();
    test_log_controller();
    // test_process_controller();
    if (common::logfp.is_open())
    {
        common::logfp.close();
    }
    closelog();
}

// int main(int argc, char **argv)
// {
//     // std::cout << "build completed\n";
//      testing::InitGoogleTest(&argc, argv);
//      return RUN_ALL_TESTS();
// }