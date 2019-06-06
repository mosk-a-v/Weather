#pragma once
#include "Common.h"
#include "SensorValues.h"
#include "CycleInfo.h"

class TemplateUtils {
    static std::string GetSensorValue(SensorValues *sensorValues, SensorId sensorId);
    static std::string GetAllSensorValues(SensorValues *sensorValues);
    static std::string FillTemplate(SensorValues *sensorValues, CycleInfo *cycle, const std::string statusTemplate, const time_t & now);
public:
    static void WriteCurrentStatus(SensorValues *sensorValues, CycleInfo *cycle, const std::string statusTemplate, const time_t & now);
};

