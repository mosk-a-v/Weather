#pragma once
#include <stdlib.h>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

#define DEFAULT_TEMPERATURE -100
#define DEFAULT_LATENCY 20
#define MAX_TEMPERATURE_DEVIATION 4
#define MAX_DELTA_DEVIATION 100
#define MIN_CYCLE_TIME 60
#define DEFAULT_TIME 0

#define OUTPUT_FILE_NAME "/var/www/html/index.html"

class CycleInfo {
    time_t cycleStartTime = DEFAULT_TIME;
    time_t cycleEndTime = DEFAULT_TIME;
    time_t lastDeviceResponceTime = DEFAULT_TIME;
    time_t lastIndoorResponceTime = DEFAULT_TIME;
    time_t lastOutdoorResponceTime = DEFAULT_TIME;
    time_t lastBoilerResponseTime = DEFAULT_TIME;
    bool isHeating = false;
    bool boilerStatus = false;
    bool isLatencyPeriod = false;
    bool isCycleEnd = false;
    float lastBoilerTemperature = DEFAULT_TEMPERATURE;
    float lastOutdoorTemperature = DEFAULT_TEMPERATURE;
    float lastIndoorTemperature = DEFAULT_TEMPERATURE;
    float requiredBoilerTemperature = DEFAULT_TEMPERATURE;
    float averageOutdoorTemperature = DEFAULT_TEMPERATURE;
    float averageIndoorTemperature = DEFAULT_TEMPERATURE;
    float delta = 0;
    float deltaForLastPeriod = 0;
    float latency = DEFAULT_LATENCY;
    string statusTemplate;
    string additionalTemplateParameterValue;

    void DetectLatency();
    void CalculateDelta(float boilerTemperature, const time_t& now);
    float CalclateDeltaForLastPeriod(float boilerTemperature, const time_t& now);
    void ApplyTemplateAndWrite(std::ostream & stream, const time_t& now);
    void EndCycle(const time_t& now);
    void ProcessBoilerTemperature(float value, const time_t & now);
    bool IsStartingMode();
    void DetectComplitingStartMode(const time_t & now);
    string FormatTime(const time_t& time);
public:
    CycleInfo(bool isHeating, float requiredBoilerTemperature, const time_t& now, string statusTemplate, string additionalTemplateParameterValue);
    void AddBoilerTemperatue(float value, const time_t& now);
    void AddIndoorTemperature(float value, const time_t& now);
    void AddOutdoorTemperature(float value, const time_t& now);
    float GetAverageIndoorTemperature();
    float GetAverageOutdoorTemperature();
    float GetBoilerTemperture();
    float GetIndoorTemperature();
    float GetOutdoorTemperature();
    float GetRequiredBoilerTemperature();
    bool IsBoilerOn();
    bool IsCycleEnd();
    void WriteCurrentStatus(const time_t& now);
    int GetCycleLength();
    bool IsCycleHeating();
};

