#ifndef LIBS_HPP
#define LIBS_HPP
#pragma once

#define PCRE2_CODE_UNIT_WIDTH 8

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

#if __linux__
#include <zlib.h>
#include <dirent.h>
#include <pugixml.hpp>
#include <pcre2.h>
#include <json/json.h>
#include <curl/curl.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/hmac.h>
#include <syslog.h>
#include <mqtt/async_client.h>

#endif

#include "constants.hpp"

using std::exception;
using std::fstream;
using std::map;
using std::string;
using std::vector;

typedef map<string, map<string, string>> config_table_type;

#endif // LIBS_HPP