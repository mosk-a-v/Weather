#pragma once
#include "Common.h"
#include "GlobalWeather.h"
#include "Storage.h"
#include "CycleInfo.h"
#include "SensorValues.h"
#include "TemplateUtils.h"

//#define TEST

static std::exception_ptr globalExceptionPtr = nullptr;

class Management {
private:
    std::mutex mu;
    std::vector<ControlValue> *controlTable;
    std::vector<SettingValue> *settingsTable;
    GlobalWeather *globalWeatherSystem;
    Storage *storage;
    float maxIndoorTemperature;
    float minIndoorTemperature;
    float maxOutdoorTemperature;
    float minOutdoorTemperature;
    std::string statusTemplate;
    CycleInfo *cycleInfo = nullptr;
    SensorValues *sensorValues = nullptr;
protected:
    virtual std::time_t GetTime();
    float GetRequiredIndoorTemperature();
    float GetRequiredBoilerTemperature(int sun, int wind, float outdoorTemperature, float indoorTemperature);
    
    float GetControlValue(int sun, int wind, float outdoorTemperature, float indoorTemperature);
    void BeginNewCycle(const time_t &now);
    void SetupGPIO();
    void SetGPIOValues();
    void ReadTemplate();
    void StoreGlobalWeather();
    SensorId GetBoilerSensorId();
    float GetAvgIndoorTemperature();
    float GetAvgOutdoorTemperature();
public:
    void ProcessResponce(const DeviceResponce& responce);
    Management(Storage *storage, GlobalWeather *globalWeatherSystem);
    ~Management();
};

