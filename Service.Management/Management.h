#include <wiringPi.h>
#include "Input.h"
#include "Storage.h"
#define	PIN	27
#define MAX_TEMPERATURE_DEVIATION 5
#define MAX_DELTA_DEVIATION 100
//#define TEST

#pragma once
class Management {
private:
    std::vector<ControlValue> *controlTable;
    std::vector<SettingValue> *settingsTable;
    short sun = 0;
    short wind = 0;
    float outdoorTemperature = 0;
    float indoorTemperature = 20;
    float maxIndoorTemperature;
    float minIndoorTemperature;
    float maxOutdoorTemperature;
    float minOutdoorTemperature;
protected:
    std::time_t lastBoilerResponseTime;
    bool isBoilerOn = false;
    float boilerTemperature = 0;
    float delta = 0;
    virtual std::time_t GetTime();
    virtual std::tm* GetDate();
    float GetRequiredIndoorTemperature();
    float GetRequiredBoilerTemperature(short sun, short wind, float outdoorTemperature, float indoorTemperature);
    float GetControlValue(short sun, short wind, float outdoorTemperature, float indoorTemperature);
    float CalclateDelta(float actualilerTemperature, float requiredBoilerTemperature, std::time_t now);
    void ManageBoiler(float actualilerTemperature, std::time_t now);
public:
    void ProcessResponce(const DeviceResponce& responce);
    Management(const std::vector<ControlValue>& controlTable, const std::vector<SettingValue>& settingsTable);
    ~Management();
};

