#ifndef cOMMAND_SERVICE
#define cOMMAND_SERVICE
#include "util/common.hpp"

class command_service
{
public:
    virtual int read_command(const string &command, vector<string> &process_name) = 0;
    virtual ~command_service() {}
};

class command_service_impl : public command_service
{
public:
    int read_command(const string &command, vector<string> &logs)
    {
        FILE *process = popen(command.c_str(), "r");
        char buffer[Audit::BUFFER_SIZE];
        if (!process)
        {
            LOG_ERROR("failed to open this process for " + command);
            return FAILED;
        }

        while (!feof(process))
        {
            if (fgets(buffer, sizeof(buffer), process) != nullptr)
            {
                logs.push_back(buffer);
            }
        }
        pclose(process);

        return Audit::SUCCESS;
    }

private:
    int validate_command(const string& command)
    {
        int result = Audit::FAILED;
        for (const string& s : whitelist)
        {
            if (strcmp(s.c_str(), command.c_str()) == 0)
            {
                result = Audit::SUCCESS;
                break;
            }
        }
        return result;
    }

private:
    vector<string> whitelist = {"netstat -tan | grep LISTEN | egrep -v '(127.0.0.1|::1)' | sort", "df -P", "last -n 5"};
};
#endif