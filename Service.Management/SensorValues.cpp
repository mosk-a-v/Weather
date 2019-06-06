#include "SensorValues.h"

int SensorValues::GetSensorIndex(SensorId id) {
    return sensorIndex[id];
}

SensorValues::SensorValues() {
    sensorIndex[RadioOutdoor] = 0;
    sensorIndex[RadioIndoor] = 1;
    sensorIndex[RadioBoiler] = 2;
    sensorIndex[DirectBoiler] = 3;
    sensorIndex[DirectIndoor] = 4;

    for(int i = 0; i < SENSORS_COUNT; i++) {
        lastSensorValues[i] = DEFAULT_TEMPERATURE;
        avgSensorValues[i] = DEFAULT_TEMPERATURE;
        lastSensorResponseTime[i] = DEFAULT_TIME;
        firstSensorResponseTime[i] = DEFAULT_TIME;
    }
}

SensorValues::~SensorValues() {
    sensorIndex.~map();
}

float SensorValues::GetAveragSensorValue(SensorId id) {
    int sensorIndex = GetSensorIndex(id);
    return (lastSensorResponseTime[sensorIndex] == DEFAULT_TIME) ?
        lastSensorResponseTime[sensorIndex] : avgSensorValues[sensorIndex] /
        (lastSensorResponseTime[sensorIndex] - firstSensorResponseTime[sensorIndex]);
}

float SensorValues::GetLastSensorValue(SensorId id) {
    int sensorIndex = GetSensorIndex(id);
    return lastSensorValues[sensorIndex];
}

time_t SensorValues::GetLastSensorResponseTime(SensorId id) {
    int sensorIndex = GetSensorIndex(id);
    return lastSensorResponseTime[sensorIndex];
}

void SensorValues::AddSensorValue(SensorId id, float value, time_t time) {
    int sensorIndex = GetSensorIndex(id);
    if(value == DEFAULT_TEMPERATURE) {
        return;
    }
    lastSensorResponseTime[sensorIndex] = value;
    if(avgSensorValues[sensorIndex] == DEFAULT_TEMPERATURE) {
        firstSensorResponseTime[sensorIndex] = time;
        avgSensorValues[sensorIndex] = value;
    } else {
        avgSensorValues[sensorIndex] += (time - lastSensorResponseTime[sensorIndex]) * value;
    }
    lastSensorResponseTime[sensorIndex] = time;
}

std::string SensorValues::ToString(SensorId id) {
    std::stringstream ss;
    ss << GetLastSensorValue(id) << " [" << Utils::FormatTime(GetLastSensorResponseTime(id)) << "] (" << GetAveragSensorValue(id) << ")";
    return ss.str();
}
