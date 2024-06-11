#ifndef MQTT_STRING_BUILDER_HPP
#define MQTT_STRING_BUILDER_HPP

#pragma once

#include "util/Common.hpp"
#include "log_collector/LogEntity.hpp"
#include "MqttEntity.hpp"
#include "monitor/MonitorEntity.hpp"

class MqttStringBuilder
{
public:
    template <typename T>
    string toJson(const T &reponse);
    Json::Value toJson(const vector<std::string> &logs, const string &name);
    Json::Value toJson(const vector<ProcessData> &logs);
};

#endif
