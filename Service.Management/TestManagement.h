#pragma once
#include "Management.h"
#ifdef TEST
class TestManagement :
    public Management {
    virtual std::tm* GetDate();
    virtual std::time_t GetTime();
    std::tm dateTime;
public:
    TestManagement(const std::vector<ControlValue>& controlTable, const std::vector<SettingValue>& settingsTable);
    ~TestManagement();
    std::time_t time = 0;
    void SetDateTime(int hour, int wDay);
    float GetRequiredIndoorTemperatureWrapper();
    float GetRequiredBoilerTemperatureWrapper(float outdoorTemperature, float indoorTemperature);
    float CalclateDeltaWrapper(float actualilerTemperature, float requiredBoilerTemperature);
    void SetBoilerTemperature(float temperature);
    float GetBoilerTemperature();
    bool GetBoilerStatus();
    void SetBoilerResponseTime(std::time_t time);
};
void AddControlValue(std::vector<ControlValue>& controlTable, float indoor, float outdoor, float boiler);
void AddSettingValue(std::vector<SettingValue>& settingsTable, int hour, int day, float temperature);
TestManagement* IninManagement();
void ProcessInput(Management *testManagement, SensorId sensor, float value);
void TestAll();

#endif // TEST

