#include <wiringPi.h>
#include <fstream>
#include <iostream>
#include "Input.h"
#include "Storage.h"
#include "CycleInfo.h"

#define	PIN	17
#define MAX_TEMPERATURE_DEVIATION 4
#define MIN_CYCLE_TIME 60
//#define TEST
#define TEMPLATE_FILE_NAME "/var/www/html/boiler_status.html"

#pragma once
class Management {
private:
    std::vector<ControlValue> *controlTable;
    std::vector<SettingValue> *settingsTable;
    GlobalWeather *globalWeatherSystem;
    float maxIndoorTemperature;
    float minIndoorTemperature;
    float maxOutdoorTemperature;
    float minOutdoorTemperature;
    string statusTemplate;
    CycleInfo *cycleInfo = nullptr;
protected:
    virtual std::time_t GetTime();
    virtual std::tm* GetDate();
    float GetRequiredIndoorTemperature();
    float GetRequiredBoilerTemperature(int sun, int wind, float outdoorTemperature, float indoorTemperature);
    static float GetAdjustBoilerTemperature(float indoorTemperature, float requiredIndoorTemperature, float requiredBoilerTemperature);
    float GetControlValue(int sun, int wind, float outdoorTemperature, float indoorTemperature);
    void ManageBoiler(float actualilerTemperature, std::time_t now);
    void BeginNewCycle(const time_t &now);
    void SetupGPIO();
    void SetGPIOValues();
    float GetSunAdjust(int sun);
    float GetWindAdjust(int sun);
public:
    void ProcessResponce(const DeviceResponce& responce);
    Management(const std::vector<ControlValue>& controlTable, const std::vector<SettingValue>& settingsTable, GlobalWeather *globalWeatherSystem);
    void ReadTemplate();
    ~Management();
};

