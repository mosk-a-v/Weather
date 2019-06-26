#pragma once
#include "Common.h"

class Utils {
public:
    static float GetAdjustBoilerTemperature(float indoorTemperature, float requiredIndoorTemperature, float requiredBoilerTemperature);
    static std::string FormatTime(const time_t& time);
    static float GetSunAdjust(int sun);
    static float GetWindAdjust(int wind);
    static void SetupGPIO();
    static void SetGPIOValues(int pin, bool value);
    static std::time_t GetTime();
};
