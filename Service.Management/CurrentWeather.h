#pragma once
#include <ctime>
#include <math.h>

#define HALF_LIGHT_TIME 3000

struct CurrentWeather {
public:
    std::time_t SunRise;
    std::time_t SunSet;
    std::time_t LastUpdateTime;
    std::time_t ResponceTime;
    float TemperatureValue;
    float TemperatureMin;
    float TemperatureMax;
    float WindSpeed = 0;
    int Clouds;
    bool IsServiceError;
    int GetSun();
    int GetWind();
    float GetTemperature();
    float GetAverageMonthTemperature(int month);
    float GetDailyShift(int hour);
};
