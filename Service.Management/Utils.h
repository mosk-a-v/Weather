#pragma once
#include "Common.h"

class Utils {
public:
    static float GetAdjustBoilerTemperature(float indoorTemperature, float requiredIndoorTemperature, float requiredBoilerTemperature);
    static std::string FormatTime(const time_t& time);
    static std::string FormatDateTime(const time_t & time);
    static float GetSunAdjust(int sun);
    static float GetWindAdjust(int wind);
    static void SetupGPIO();
    static void SetGPIOValues(int pin, bool value);
    static std::time_t GetTime();
    static void WriteLogInfo(int priority, std::string message);
    static std::string ReadFile(std::string fileName);
    static bool CompareChar(const char &c1, const char &c2);
    static bool CaseInSensStringCompare(const std::string& str1, const std::string& str2);
};
