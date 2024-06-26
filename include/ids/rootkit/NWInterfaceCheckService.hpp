#ifndef NETWORK_INTERFACE_CHECK_HPP
#define NETWORK_INTERFACE_CHECK_HPP
#pragma once
#include "util/Common.hpp"
/*
    Commmand :: "ifConfigService %s | grep PROMISC > /dev/null 2>&1" :: To identify the interface is in promiscuous mode
    steps    :: Get all the available interfaces.
*/
class NwInterfaceCheckService
{
public:
    NwInterfaceCheckService():fd(0), errors(0),total(0) {}

    int isInterfaceInPromiscuousMode(const std::string& interfaceName)
    {
        char nt[SCM::OS_SIZE_1024 + 1];
        snprintf(nt, SCM::OS_SIZE_1024, SCM::PROMISCOUS.c_str(), interfaceName.c_str());
        if (system(nt) == 0) {
            return (1);
        }
        return (0);
    }

    int check(vector<string> & reports)
    {
        DEBUG("checking network interfaces starting");
        fd = socket(AF_INET, SOCK_DGRAM, 0);
 
        if (fd < 0)
        {
            LOG_ERROR("error checking interfaces (socket)");
            return SCM::FAILED;
        }
        memset(tmp_str, 0, sizeof(struct ifreq) * 16);
        _if.ifc_len = sizeof(tmp_str);
        _if.ifc_buf = (caddr_t)(tmp_str);
 
        if (ioctl(fd, SIOCGIFCONF, &_if) < 0)
        {
            close(fd);
            LOG_ERROR("error checking interfaces (ioctl)");
            return SCM::FAILED;
        }
 
        _ifend = (struct ifreq *) (void *) ((char *)tmp_str + _if.ifc_len);
        _ir = tmp_str;
 
        for (; _ir < _ifend; _ir++) 
        {
            strncpy(_ifr.ifr_name, _ir->ifr_name, sizeof(_ifr.ifr_name));
            // Get information from each interface
            if (ioctl(fd, SIOCGIFFLAGS, (char *)&_ifr) == -1) {
                continue;
            }
 
            total++;
 
            if ((_ifr.ifr_flags & IFF_PROMISC)) {
                char op_msg[SCM::OS_SIZE_1024];
                // Check if the interface is in promiscuous mode
                if (isInterfaceInPromiscuousMode(_ifr.ifr_name)) {
                    // Assume we have a function to notify about the promiscuous interface.
                    snprintf(op_msg, SCM::OS_SIZE_1024, "Interface '%s' in promiscuous"
                         " mode.", _ifr.ifr_name);
                }
                else
                {
                    snprintf(op_msg, SCM::OS_SIZE_1024, "Interface   '%s' in promiscuous"
                         " mode, but ifConfigService is not showing it"
                         "(probably trojaned).", _ifr.ifr_name);
                }
                reports.push_back(_ifr.ifr_name);
                LOG_ERROR(op_msg);
                errors++;
            }
        }
        close(fd);
 
        if (errors == 0) {
            DEBUG("no problem detected on ifConfigService/ifs. analyzed " + std::to_string(total) + " interfaces");
        }
 
        return SCM::SUCCESS;
    }
private:
    const string PROMISCOUS = "ifConfigService %s | grep PROMISC > /dev/null 2>&1";
    vector<string> promiscuousInterfaces;
    int fd; /* File descriptor */
    int errors;
    int total;
    struct ifreq tmp_str[16];
    struct ifconf _if;
    struct ifreq* _ir;
    struct ifreq* _ifend;
    struct ifreq _ifr;
 
};
 
#endif //NETWORK_INTERFACE_CHECK_HPP
