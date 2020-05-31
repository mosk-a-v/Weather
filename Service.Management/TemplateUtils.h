#pragma once
#include "Common.h"
#include "SensorValues.h"
#include "CycleInfo.h"
#include "MqttPublisher.h"

class TemplateUtils {
    static std::string FillTemplate(SensorValues *sensorValues, CycleInfo *cycle, MqttPublisher* publisher, const std::string statusTemplate, char *additionalInfo, const time_t & now);
public:
    static void WriteCurrentStatus(SensorValues *sensorValues, CycleInfo *cycle, MqttPublisher* publisher, const std::string statusTemplate, char *additionalInfo, const time_t & now);
};

class IntegerNumPunct : public std::numpunct<char> {
protected:
    virtual char_type do_thousands_sep() const { return '`'; }
    virtual std::string do_grouping() const { return "\03"; }
};
