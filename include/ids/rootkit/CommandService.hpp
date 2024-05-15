#ifndef COMMAND_SERVICE_HPP
#define COMMAND_SERVICE_HPP
#include "util/Common.hpp"

class CommandService
{
public:
    virtual int readCommand(const string &command, vector<string> &process_name) = 0;
    virtual ~CommandService() {}
};

class CommandServiceImpl : public CommandService
{
public:
    int readCommand(const string &command, vector<string> &logs)
    {
        FILE *process = popen(command.c_str(), "r");
        char buffer[SCM::BUFFER_SIZE];
        if (!process)
        {
            LOG_ERROR("failed to open this process for " + command);
            return SCM::FAILED;
        }

        while (!feof(process))
        {
            if (fgets(buffer, sizeof(buffer), process) != nullptr)
            {
                logs.push_back(buffer);
            }
        }
        pclose(process);

        return SCM::SUCCESS;
    }

private:
    int validateCommand(const string& command)
    {
        int result = SCM::FAILED;
        for (const string& s : whitelist_commands)
        {
            if (strcmp(s.c_str(), command.c_str()) == 0)
            {
                result = SCM::SUCCESS;
                break;
            }
        }
        return result;
    }

private:
    const vector<string> whitelist_commands = {"netstat -tan | grep LISTEN | egrep -v '(127.0.0.1|::1)' | sort", "df -P", "last -n 5"};
};

#endif //COMMAND_SERVICE_HPP