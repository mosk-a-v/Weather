#pragma once
#include "Common.h"

class CycleInfo {
    CycleResult result;

    time_t cycleStartTime = DEFAULT_TIME;
    time_t cycleEndTime = DEFAULT_TIME;
    time_t lastBoilerResponceTime = DEFAULT_TIME;

    bool isLatencyPeriod = false;
    bool isCycleEnd = false;
    bool isHeating = false;

    float delta = 0;
    float deltaForLastPeriod = 0;
    float maxDelta = 0;
    float latency = DEFAULT_LATENCY;
    float requiredBoilerTemperature = DEFAULT_TEMPERATURE;
    float lastBoilerTemperature = DEFAULT_TEMPERATURE;

    void DetectLatency();
    void CalculateDelta(float boilerTemperature, const time_t& now);
    float CalclateDeltaForLastPeriod(float boilerTemperature, const time_t& now);
    void EndCycle(CycleResult result, const time_t& now);
    void DetectComplitingStartMode(const time_t & now);
    bool IsStartingMode();
public:
    CycleInfo(bool isHeating, float requiredBoilerTemperature,  float currentBoilerTemperature, const time_t& now);
    ~CycleInfo();
    void ProcessBoilerTemperature(float value, const time_t& now);
    bool IsCycleEnd();
    bool IsBoilerOn();
    CycleStatictics* GetStatictics();
};