#pragma once
#include "SensorValues.h"
#include "TemperatureStrategyBase.h"

class DirectTemperatureStrategy : public TemperatureStrategyBase {
private:
    float boilerTemperature;
public:
    DirectTemperatureStrategy(float boilerTemperature);
    virtual ~DirectTemperatureStrategy();

    float GetBoilerTemperature(SensorValues * lastSensorValues, float sun, float wind);
    SensorId GetBoilerSensorId(SensorValues * sensorValues);
};

