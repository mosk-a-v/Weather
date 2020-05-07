#pragma once
#include "Common.h"
#include "SensorValues.h"
#include "Storage.h"

class TemperatureStrategyBase {
protected:
    std::map<std::string, SensorInfo> *sensorsTable = nullptr;
    void LoadSensorTable();
public:
    TemperatureStrategyBase();
    virtual ~TemperatureStrategyBase();

    virtual float GetBoilerTemperature(SensorValues * lastSensorValues, float sun, float wind) = 0;
    virtual SensorId GetBoilerSensorId(SensorValues * sensorValues) = 0;
    float GetIndoorTemperature(SensorValues * sensorValues);
    float GetOutdoorTemperature(SensorValues * sensorValues);
};

