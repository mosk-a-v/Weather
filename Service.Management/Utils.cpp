#include "Utils.h"

float Utils::GetAdjustBoilerTemperature(float indoorTemperature, float requiredIndoorTemperature, float requiredBoilerTemperature) {
    if(indoorTemperature < requiredIndoorTemperature - 0.6)
        return requiredBoilerTemperature + 8;
    else if(indoorTemperature < requiredIndoorTemperature - 0.4)
        return requiredBoilerTemperature + 6;
    else if(indoorTemperature < requiredIndoorTemperature - 0.2)
        return requiredBoilerTemperature + 4;
    else if(indoorTemperature < requiredIndoorTemperature - 0.05)
        return requiredBoilerTemperature + 2;
    else if(indoorTemperature < requiredIndoorTemperature + 0.05)
        return requiredBoilerTemperature;
    else if(indoorTemperature < requiredIndoorTemperature + 0.2)
        return requiredBoilerTemperature - 2;
    else if(indoorTemperature < requiredIndoorTemperature + 0.4)
        return requiredBoilerTemperature - 4;
    else if(indoorTemperature < requiredIndoorTemperature + 0.6)
        return requiredBoilerTemperature - 6;
    return requiredBoilerTemperature - 8;
}

std::string Utils::FormatTime(const time_t & time) {
    std::stringstream buffer;
    tm tm = *localtime(&time);
    buffer.imbue(std::locale("ru_RU.utf8"));
    buffer << std::put_time(&tm, "%T");
    return buffer.str();
}

float Utils::GetSunAdjust(int sun) {
    if(sun < 30)
        return 0;
    else if(sun < 50)
        return 1;
    else if(sun < 70)
        return 2;
    else if(sun < 80)
        return 3;
    else if(sun < 90)
        return 4;
    else
        return 5;
}

float Utils::GetWindAdjust(int wind) {
    if(wind < 10)
        return 0;
    else if(wind < 20)
        return 2;
    else if(wind < 30)
        return 3;
    else
        return 4;
}

void Utils::SetupGPIO() {
    wiringPiSetupGpio();
    pinMode(BOILER_STATUS_PIN, OUTPUT);
    pinMode(SENSOR_POWER_PIN, OUTPUT);
    digitalWrite(BOILER_STATUS_PIN, true);
    digitalWrite(SENSOR_POWER_PIN, false);
}

void Utils::SetGPIOValues(int pin, bool value) {
    //std::lock_guard<std::mutex> lock(gpio_lock);
    digitalWrite(pin, value);
}

std::time_t Utils::GetTime() {
    return std::time(0);
}