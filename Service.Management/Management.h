#pragma once
#include "Common.h"
#include "GlobalWeather.h"
#include "Storage.h"
#include "CycleInfo.h"
#include "SensorValues.h"
#include "TemplateUtils.h"

//#define TEST

class Management {
private:
    std::vector<ControlValue> *controlTable = nullptr;
    std::vector<SettingValue> *settingsTable = nullptr;
    std::map<std::string, SensorInfo> *sensorsTable = nullptr;
    GlobalWeather *globalWeatherSystem = nullptr;;
    Storage *storage = nullptr;
    CycleInfo *cycleInfo = nullptr;
    SensorValues *sensorValues = nullptr;
    char *additionalInfo = new char[250];
    float maxIndoorTemperature;
    float minIndoorTemperature;
    float maxOutdoorTemperature;
    float minOutdoorTemperature;
    float latency = DEFAULT_LATENCY;
    std::string statusTemplate;
    SensorId boilerSensorId = DirectBoiler;
protected:
    float GetRequiredIndoorTemperature();
    float GetRequiredBoilerTemperature(int sun, int wind, float outdoorTemperature, float indoorTemperature);
    float GetControlValue(int sun, int wind, float outdoorTemperature, float indoorTemperature);
    void BeginNewCycle(const time_t &now);
    void ReadTemplate();
    void StoreGlobalWeather();
    bool SetBoilerSensorId(SensorValues *sensorValues);
    float GetIndoorTemperature(SensorValues *sensorValues);
    float GetOutdoorTemperature(SensorValues *sensorValues);
public:
    void LoadControlTable();
    void LoadSettingsTable();
    void ProcessResponce(const DeviceResponce& responce);
    Management(Storage *storage, GlobalWeather *globalWeatherSystem, std::map<std::string, SensorInfo> *sensorsTable);
    ~Management();
};

