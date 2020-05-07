#pragma once
#include "SensorValues.h"
#include "TemperatureStrategyBase.h"

class NormalTemperatureStrategy : public TemperatureStrategyBase {
protected:
    float maxIndoorTemperature;
    float minIndoorTemperature;
    float maxOutdoorTemperature;
    float minOutdoorTemperature;
    std::vector<ControlValue> *controlTable = nullptr;
    std::vector<SettingValue> *settingsTable = nullptr;
    
    void LoadControlTable();
    void LoadSettingsTable();
    float GetRequiredIndoorTemperature();
    float GetRequiredBoilerTemperature(int sun, int wind, float outdoorTemperature, float indoorTemperature);
    float GetControlValue(int sun, int wind, float outdoorTemperature, float indoorTemperature);
public:
    NormalTemperatureStrategy();
    virtual ~NormalTemperatureStrategy();

    float GetBoilerTemperature(SensorValues * lastSensorValues, float sun, float wind);
    SensorId GetBoilerSensorId(SensorValues * sensorValues);
};

