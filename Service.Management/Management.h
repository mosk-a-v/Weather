#pragma once

#include <wiringPi.h>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>
#include <future>
#include "Input.h"
#include "Storage.h"
#include "CycleInfo.h"

#define	PIN	17
//#define TEST
#define TEMPLATE_FILE_NAME "/var/www/html/boiler_status.html"

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
    string statusTemplate;
    CycleInfo *cycleInfo = nullptr;
protected:
    virtual std::time_t GetTime();
    float GetRequiredIndoorTemperature();
    float GetRequiredBoilerTemperature(int sun, int wind, float outdoorTemperature, float indoorTemperature);
    static float GetAdjustBoilerTemperature(float indoorTemperature, float requiredIndoorTemperature, float requiredBoilerTemperature);
    float GetControlValue(int sun, int wind, float outdoorTemperature, float indoorTemperature);
    void ManageBoiler(std::time_t now);
    void BeginNewCycle(const time_t &now);
    void SetupGPIO();
    void SetGPIOValues();
    float GetSunAdjust(int sun);
    float GetWindAdjust(int sun);
    void ReadTemplate();
public:
    void ProcessResponce(const DeviceResponce& responce);
    Management(Storage *storage, GlobalWeather *globalWeatherSystem);
    ~Management();
};

