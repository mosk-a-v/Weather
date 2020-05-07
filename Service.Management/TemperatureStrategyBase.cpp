#include "TemperatureStrategyBase.h"

float TemperatureStrategyBase::GetIndoorTemperature(SensorValues *sensorValues) {
    float avg = 0;
    int count = 0;
    for(auto it = sensorsTable->begin(); it != sensorsTable->end(); ++it) {
        SensorInfo sensor = it->second;
        if(sensor.IsIndoor && sensor.UseForCalc) {
            if(sensorValues->GetLastSensorResponseTime(sensor.Id) != DEFAULT_TIME && !sensorValues->IsSensorWarning(sensor.Id)) {
                avg += sensorValues->GetLastSensorValue(sensor.Id);
                count++;
            }
        }
    }
    return count != 0 ? (avg / count) : DEFAULT_TEMPERATURE;
}
float TemperatureStrategyBase::GetOutdoorTemperature(SensorValues *sensorValues) {
    float avg = 0;
    int count = 0;
    for(auto it = sensorsTable->begin(); it != sensorsTable->end(); ++it) {
        SensorInfo sensor = it->second;
        if(sensor.IsOutdoor && sensor.UseForCalc) {
            if(sensorValues->GetLastSensorResponseTime(sensor.Id) != DEFAULT_TIME && !sensorValues->IsSensorWarning(sensor.Id)) {
                avg += sensorValues->GetLastSensorValue(sensor.Id);
                count++;
            }
        }
    }
    return count != 0 ? (avg / count) : sensorValues->GetAverageSensorValue(GlobalOutdoor);
}
void TemperatureStrategyBase::LoadSensorTable() {
    if(sensorsTable != nullptr) {
        delete sensorsTable;
        sensorsTable = nullptr;
    }
    sensorsTable = Storage::ReadSensorsTable();
}
TemperatureStrategyBase::TemperatureStrategyBase() {
    LoadSensorTable();
}
TemperatureStrategyBase::~TemperatureStrategyBase() {
    if(sensorsTable != nullptr) {
        delete sensorsTable;
        sensorsTable = nullptr;
    }
}

