// #ifndef LOGGER_HPP
// #define LOGGER_HPP

// #pragma once

// #include "libs.hpp"
// #include <sstream>
// #include <iostream>

// // ANSI color escape codes
// #define ANSI_COLOR_RED     "\x1b[31m"
// #define ANSI_COLOR_YELLOW  "\x1b[33m"
// #define ANSI_COLOR_GREEN   "\x1b[32m"
// #define ANSI_COLOR_RESET   "\x1b[0m"

// #if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
// template<typename... Args>
// void LOG_COLORED(const char* file, const int line, const char* function,const char* color, Args&&... args) {
//     std::ostringstream log_stream;
//     log_stream << color;
//     log_stream << "[" << file << ":" << line << " " << function << "] ";
//     (log_stream << ... << args);
//     log_stream << ANSI_COLOR_RESET << std::endl;
//     std::cout << log_stream.str();
// }
// #else
// #define LOG_COLORED(color, ...) \
//     do { \
//         std::ostringstream log_stream; \
//         log_stream  << color; \
//         log_stream << "[" << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << "] "; \
//         log_stream << __VA_ARGS__; \
//         log_stream << ANSI_COLOR_RESET << std::endl; \
//         std::cout << log_stream.str(); \
//     } while(0)
// #endif

// // Macros for colored logging
// #define LOG_ERROR(...)    LOG_COLORED(__FILE__,__LINE__,__FUNCTION__ ,ANSI_COLOR_RED, "[ERROR] ", __VA_ARGS__)
// #define DEBUG(...)    LOG_COLORED(__FILE__,__LINE__,__FUNCTION__ ,ANSI_COLOR_YELLOW, "[DEBUG] ", __VA_ARGS__)
// #define LOG(...)          LOG_COLORED(__FILE__,__LINE__,__FUNCTION__ ,ANSI_COLOR_GREEN, "[SUCCESS] ", __VA_ARGS__)

// #endif // LOGGER_HPP

#ifndef LOGGER_HPP
#define LOGGER_HPP

#pragma once

#include "libs.hpp"
#include <sstream>
#include <iostream>
#include <fstream>
#include "util/common.hpp"

// ANSI color escape codes
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"

extern int debug_enabled;
extern int console_log_enabled;

inline bool DEBUG_ENABLED() { return debug_enabled; }
inline bool CONSOLE_LOG_ENABLED() { return console_log_enabled; }

#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
template <typename... Args>
void LOG_COLORED(const char *file, const int line, const char *function, const char *color, Args &&...args)
{
    std::ostringstream log_stream;
    if (CONSOLE_LOG_ENABLED())
    {
        log_stream << color;
        log_stream << "[" << file << ":" << line << " " << function << "] ";
        (log_stream << ... << args);
        log_stream << ANSI_COLOR_RESET << std::endl;
        std::cout << log_stream.str();
    }
    else
    {
        log_stream << "[" << file << ":" << line << " " << function << "] ";
        (log_stream << ... << args);
    }
}
#else
#define LOG_COLORED(color, ...)                                                          \
    do                                                                                   \
    {                                                                                    \
        std::ostringstream log_stream;                                                   \
        log_stream << color;                                                             \
        log_stream << "[" << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << "] "; \
        log_stream << __VA_ARGS__;                                                       \
        log_stream << ANSI_COLOR_RESET << std::endl;                                     \
        if (CONSOLE_LOGGING_ENABLED)                                                     \
        {                                                                                \
            std::cout << log_stream.str();                                               \
        }                                                                                \
        if (DEBUG_ENABLED())                                                             \
        {                                                                                \
    // Write to file \
            // Implement file writing logic here \
        } \
    } while(0)
#endif

#define LOG_ERROR(...) LOG_COLORED(__FILE__, __LINE__, __FUNCTION__, ANSI_COLOR_RED, "[ERROR] ", __VA_ARGS__)
#define DEBUG(...)                                                                                 \
    if (DEBUG_ENABLED())                                                                           \
    {                                                                                              \
        LOG_COLORED(__FILE__, __LINE__, __FUNCTION__, ANSI_COLOR_YELLOW, "[DEBUG] ", __VA_ARGS__); \
    }
#define LOG(...) LOG_COLORED(__FILE__, __LINE__, __FUNCTION__, ANSI_COLOR_GREEN, "[SUCCESS] ", __VA_ARGS__)

#endif // LOGGER_HPP
