#pragma once
#include "Common.h"
#include "Utils.h"

class SensorValues {
    std::map<int, int> sensorIndex;
    float lastSensorValues[SENSORS_COUNT];
    float avgSensorValues[SENSORS_COUNT];
    float minSensorValues[SENSORS_COUNT];
    float maxSensorValues[SENSORS_COUNT];
    bool sensorWarnings[SENSORS_COUNT];
    time_t lastSensorResponseTime[SENSORS_COUNT];
    time_t firstSensorResponseTime[SENSORS_COUNT];
    int GetSensorIndex(SensorId id);
    void AddSensorValue(SensorId id, float value, bool warning, time_t time, bool checkDeviation);
public: 
    SensorValues();
    ~SensorValues();
    float GetAverageSensorValue(SensorId id);
    float GetLastSensorValue(SensorId id);
    float GetMinSensorValue(SensorId id);
    float GetMaxSensorValue(SensorId id);
    bool IsSensorWarning(SensorId id);
    time_t GetLastSensorResponseTime(SensorId id);
    void AddSensorValue(SensorId id, float value, bool warning, time_t time);
    void AddSpecialSensorValue(SensorId id, float value, time_t time);
    std::string ToString(SensorId id);
    std::string GetSensorColumns();
    std::string GetSensorValues();
    nlohmann::json ToJson();
    void CloneLastValues(SensorValues* lastValues);
};

