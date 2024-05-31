// #include "test/os_test.hpp"
// #include "test/ConfigService_test.hpp"

// #include "test/Common_unit_test.hpp"
// #include "test/monitor_test.hpp"
// #include "test/ids_test.hpp"
// #include "test/log_unit_test.hpp"
// #include "rest/RestService.hpp"
// #include "tpm/tpm_service.hpp"

// #include "test/mqtt_service_unit_test.hpp"
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

// #include "mqtt/MqttController.hpp"

#include "log_collector/LogServiceImpl.hpp"

static void read_gz_file(const std::string& gzFilePath)
{
    gzFile file = gzopen(gzFilePath.c_str(), "rb");
    if (!file)
    {
        std::cerr << "Cannot open gzip file: " << gzFilePath << std::endl;
        return;
    }
 
    char buffer[8192];
    int bytesRead;
 
    while ((bytesRead = gzread(file, buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[bytesRead] = '\0'; // Null-terminate the buffer
        std::cout << buffer;
    }
 
    if (bytesRead < 0)
    {
        int err;
        const char *errorString = gzerror(file, &err);
        if (err == Z_ERRNO)
        {
            perror("Error reading file");
        }
        else
        {
            std::cerr << "Error reading gzip file: " << errorString << std::endl;
        }
    }
 
    gzclose(file);
}