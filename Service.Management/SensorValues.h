#pragma once
#include "Common.h"

class SensorValues {
    std::map<int, int> sensorIndex;
    float lastSensorValues[SENSORS_COUNT];
    float avgSensorValues[SENSORS_COUNT];
    bool sensorWarnings[SENSORS_COUNT];
    time_t lastSensorResponseTime[SENSORS_COUNT];
    time_t firstSensorResponseTime[SENSORS_COUNT];
    int GetSensorIndex(SensorId id);
public: 
    SensorValues();
    ~SensorValues();
    float GetAverageSensorValue(SensorId id);
    float GetLastSensorValue(SensorId id);
    bool GetSensorWarning(SensorId id);
    time_t GetLastSensorResponseTime(SensorId id);
    void AddSensorValue(SensorId id, float value, bool warning, time_t time);
    std::string ToString(SensorId id);
    std::string GetSensorColumns();
    std::string GetSensorValues();
};

