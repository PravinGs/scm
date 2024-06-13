#ifndef LIBS_HPP
#define LIBS_HPP
#pragma once

#define PCRE2_CODE_UNIT_WIDTH 8

#pragma warning(disable : 4996)

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <set>
#include <vector>
#include <memory>
#include <filesystem>
#include <exception>
#include <mutex>
#include <ctime>
#include <regex>
#include <future>
#include <chrono>
#include <thread>
#include <time.h>

#if __linux__
#include <zlib.h>
#include <pugixml.hpp>
#include <pcre2.h>
#include <json/json.h>
#include <curl/curl.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/hmac.h>
#include <mqtt/async_client.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <pwd.h>
#include <syslog.h>

#include <fcntl.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

extern "C"
{
    #include <tss2/tss2_esys.h>
    #include <tss2/tss2_tcti_device.h>
}

#endif

#include "constants.hpp"

using std::exception;
using std::fstream;
using std::map;
using std::string;
using std::vector;

typedef map<string, map<string, string>> config_table_type;

#endif // LIBS_HPP