#include <stdlib.h>
#include <string>
using namespace std;

#define DEFAULT_TEMPERATURE -100
#define DEFAULT_LATENCY 20

#pragma once
class CycleInfo {
    time_t lastBoilerResponseTime;
    time_t cycleStartTime;
    bool isHeating = false;
    bool boilerStatus = false;
    bool isLatencyPeriod = false;
    float boilerTemperature = 0;
    float outdoorTemperature = DEFAULT_TEMPERATURE;
    float indoorTemperature = DEFAULT_TEMPERATURE;
    float delta = 0;
    float deltaForLastPeriod = 0;
    float latency = DEFAULT_LATENCY;

public:
    CycleInfo();
    ~CycleInfo();
};

