#include "DirectTemperatureStrategy.h"

DirectTemperatureStrategy::DirectTemperatureStrategy(float boilerTemperature) : TemperatureStrategyBase() {
    this->boilerTemperature = boilerTemperature;
}
DirectTemperatureStrategy::~DirectTemperatureStrategy() {
}
float DirectTemperatureStrategy::GetBoilerTemperature(SensorValues * lastSensorValues, float sun, float wind) {
    return boilerTemperature;
}
SensorId DirectTemperatureStrategy::GetBoilerSensorId(SensorValues * sensorValues) {
    return DirectBoiler;
}
