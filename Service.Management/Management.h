#include <wiringPi.h>
#include <fstream>
#include <iostream>
#include "Input.h"
#include "Storage.h"

#define	PIN	17
#define MAX_TEMPERATURE_DEVIATION 4
#define MAX_DELTA_DEVIATION 100
#define MIN_CYCLE_TIME 60
//#define TEST
#define DEFAULT_TEMPERATURE -100
#define DEFAULT_LATENCY 20
#define DEFAULT_TIME 0
#define OUTPUT_FILE_NAME "/var/www/html/index.html"
#define TEMPLATE_FILE_NAME "/var/www/html/boiler_status.html"

#pragma once
class Management {
private:
    std::vector<ControlValue> *controlTable;
    std::vector<SettingValue> *settingsTable;
    GlobalWeather *globalWeatherSystem;
    short sun = 0;
    short wind = 0;
    float maxIndoorTemperature;
    float minIndoorTemperature;
    float maxOutdoorTemperature;
    float minOutdoorTemperature;
protected:
    std::time_t lastBoilerResponseTime = DEFAULT_TIME;
    std::time_t heatingCycleSwitchTime = DEFAULT_TIME;
    string statusTemplate;
    bool isHeating = false;
    bool boilerStatus = false;
    bool isLatencyPeriod = false;
    float boilerTemperature = 0;
    float outdoorTemperature = DEFAULT_TEMPERATURE;
    float indoorTemperature = DEFAULT_TEMPERATURE;
    float delta = 0;
    float deltaForLastPeriod = 0;
    float latency = DEFAULT_LATENCY;
    virtual std::time_t GetTime();
    virtual std::tm* GetDate();
    float GetRequiredIndoorTemperature();
    float GetRequiredBoilerTemperature(short sun, short wind, float outdoorTemperature, float indoorTemperature);
    float GetControlValue(short sun, short wind, float outdoorTemperature, float indoorTemperature);
    float GetAdjustBoilerTemperature(float requiredBoilerTemperature, float requiredIndoorTemperature);
    void WriteCurrentStatus(float requiredIndoorTemperature, float requiredBoilerTemperature, float adjustBoilerTemperature);
    void ApplyTemplateAndWrite(std::ostream &stream, float requiredBoilerTemperature, float adjustBoilerTemperature);
    float CalclateDeltaForLastPeriod(float actualilerTemperature, float requiredBoilerTemperature, std::time_t now);
    void ManageBoiler(float actualilerTemperature, std::time_t now);
    void CalculateDelta(float actualilerTemperature, float adjustBoilerTemperature, const time_t &now);
    void DetectLatency();
    void BeginNewCycle(const time_t &now);
    void SetupGPIO();
    void SetGPIOValues();
public:
    void ProcessResponce(const DeviceResponce& responce);
    Management(const std::vector<ControlValue>& controlTable, const std::vector<SettingValue>& settingsTable, GlobalWeather *globalWeatherSystem);
    void ReadTemplate();
    ~Management();
};

