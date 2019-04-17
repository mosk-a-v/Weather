#include "CurrentWeather.h"

int CurrentWeather::GetSun() {
    std::time_t currentTime = std::time(0);
    if(IsServiceError) {
        tm tm = *localtime(&currentTime);
        if(tm.tm_hour > 10 && tm.tm_hour < 16) {
            return 40;
        } else {
            return 0;
        }
    } else {
        if(currentTime > (SunRise + HALF_LIGHT_TIME) && currentTime < (SunSet - HALF_LIGHT_TIME)) {
            return 100 - Clouds;
        } else {
            return 0;
        }
    }
}
int CurrentWeather::GetWind() {
    if(IsServiceError) {
        return 3;
    } else {
        return round(WindSpeed);
    }
}
float CurrentWeather::GetTemperature() {
    std::time_t currentTime = std::time(0);
    if(IsServiceError) {
        tm tm = *localtime(&currentTime);
        float avgMonth = GetAverageMonthTemperature(tm.tm_mon);
        return avgMonth + GetDailyShift(tm.tm_hour);
    } else {
        return TemperatureValue;
    }
}
float CurrentWeather::GetAverageMonthTemperature(int month) {
    float avgMonth = 0;
    switch(month) {
        case 0:
            avgMonth = -9.4f;
        case 1:
            avgMonth = -8.7f;
        case 2:
            avgMonth = -3.3f;
        case 3:
            avgMonth = 6.3f;
        case 4:
            avgMonth = 13.5f;
        case 5:
            avgMonth = 17.2f;
        case 6:
            avgMonth = 18.6f;
        case 7:
            avgMonth = 17.2f;
        case 8:
            avgMonth = 11.8f;
        case 9:
            avgMonth = 5.2f;
        case 10:
            avgMonth = -1.5f;
        case 11:
            avgMonth = -6.1f;
    }
}
float CurrentWeather::GetDailyShift(int hour) {
    if(hour < 4) {
        return -2 - hour * 0.5f;
    } else if(hour < 12) {
        return -4 + (hour - 4) * 1.0f;
    } else {
        return 4 - (hour - 12) * 0.5f;
    }
}

