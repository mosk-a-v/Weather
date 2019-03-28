#pragma once
#include <stdlib.h>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <systemd/sd-journal.h>
#include "GlobalWeather.h"

using namespace std;

#define DEFAULT_TEMPERATURE -100
#define DEFAULT_LATENCY 25
#define MAX_DELTA_DEVIATION 100
#define MIN_CYCLE_TIME 120
#define MAX_CYCLE_TIME 3000
#define DEFAULT_TIME 0
#define MAX_TEMPERATURE_DEVIATION 4

#define OUTPUT_FILE_NAME "/var/www/html/index.html"

enum CycleResult {
    Starting = 0,
    Normal = 1,
    TimeOut = 2,
    TemperatureLimit = 3
};

struct CycleStatictics {
public:
    time_t CycleStart;
    float AvgIndoor;
    float AvgOutdoor;
    float AvgBoiler;
    int Wind;
    int Sun;
    float BoilerRequired;
    bool IsHeating;
    int CycleLength;
    float LastIndoor;
    float LastOutdoor;
    float LastBoiler;
    CycleResult Result;
};

class CycleInfo {
    time_t cycleStartTime = DEFAULT_TIME;
    time_t cycleEndTime = DEFAULT_TIME;
    time_t lastDeviceResponceTime = DEFAULT_TIME;
    time_t lastIndoorResponceTime = DEFAULT_TIME;
    time_t lastOutdoorResponceTime = DEFAULT_TIME;
    time_t lastBoilerResponseTime = DEFAULT_TIME;

    bool isLatencyPeriod = false;
    bool isCycleEnd = false;
    float lastBoilerTemperature = DEFAULT_TEMPERATURE;
    float lastOutdoorTemperature = DEFAULT_TEMPERATURE;
    float lastIndoorTemperature = DEFAULT_TEMPERATURE;
    float averageOutdoorTemperature = DEFAULT_TEMPERATURE;
    float averageIndoorTemperature = DEFAULT_TEMPERATURE;
    float averageBoilerTemperature = DEFAULT_TEMPERATURE;
    float delta = 0;
    float deltaForLastPeriod = 0;
    float latency = DEFAULT_LATENCY;
    CycleResult result;

    bool isHeating = false;
    float requiredBoilerTemperature = DEFAULT_TEMPERATURE;
    string statusTemplate;
    string additionalTemplateParameterValue;
    int sun;
    int wind;

    void DetectLatency();
    void CalculateDelta(float boilerTemperature, const time_t& now);
    float CalclateDeltaForLastPeriod(float boilerTemperature, const time_t& now);
    void ApplyTemplateAndWrite(std::ostream & stream, const time_t& now);
    void EndCycle(CycleResult result, const time_t& now);
    void ProcessBoilerTemperature(float value, const time_t & now);
    bool IsStartingMode();
    void DetectComplitingStartMode(const time_t & now);
    string FormatTime(const time_t& time);
    void WriteCurrentStatus(const time_t& now);
    float GetAverageIndoorTemperature();
    float GetAverageOutdoorTemperature();
    float GetAverageBoilerTemperature();
public:
    CycleInfo(bool isHeating, float requiredBoilerTemperature, CurrentWeather weather, const time_t& now, string statusTemplate, string additionalTemplateParameterValue);
    void AddBoilerTemperatue(float value, const time_t& now);
    void AddIndoorTemperature(float value, const time_t& now);
    void AddOutdoorTemperature(float value, const time_t& now);
    bool IsBoilerOn();
    bool IsCycleEnd();
    CycleStatictics* GetStatictics();
};