cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Include directories
include_directories(
    include
    include/config
    include/ids
    include/ids/rootkit
    include/log_collector
    include/monitor
    include/mqtt
    include/patch
    include/rest
    include/tpm
    include/test
    include/util
    
)

# Project name (without spaces)
project (Secure_Components)

# Check if Google Test is available
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    message(STATUS "Google Test not found. Fetching and building it...")
    # Use FetchContent to automatically download and build Google Test
    include(FetchContent)
    FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG        release-1.11.0 # Specify the version tag here
    )
    FetchContent_MakeAvailable(googletest)
endif()

# Find OpenSSL package
find_package(OpenSSL REQUIRED)

# Add source to this project's executable.
add_executable (SCM 
    scm.cpp 
    include/mqtt/mqtt_service.cpp
    include/util/os.cpp 
    include/util/common.cpp 
    include/ids/ids_service_impl.cpp
)

# Define the header file for which precompiled header needs to be created
set(PCH_HEADER include/util/libs.hpp)

# Create a precompiled header target
target_precompile_headers(SCM PRIVATE ${PCH_HEADER})

# Add coverage flags for test coverage
# target_compile_options(SCM PRIVATE --coverage)
# target_link_options(SCM PRIVATE --coverage)

add_custom_target(
    cppcheck
    COMMAND cppcheck
        --enable=all
        --std=c++17
        --language=c++
        --suppress=unusedFunction
        --inconclusive
        --force
        --inline-suppr
        --template=gcc
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/scm.cpp
    COMMENT "Running cppcheck"
)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Link with Google Test library, OpenSSL, and zlib
target_link_libraries(SCM
    gtest
    gtest_main
    gmock
    gmock_main
    gtest
    ssl
    crypto
    pthread
    z# Assuming you have zlib installed and found by CMake
    curl
    pugixml
    jsoncpp
    pcre2-8
    tss2-esys
    tss2-tctildr
    tss2-rc
    sw-tpm
    paho-mqttpp3 
    paho-mqtt3a
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET SCM PROPERTY CXX_STANDARD 17)
endif()

# TODO: Add tests and install targets if needed.
