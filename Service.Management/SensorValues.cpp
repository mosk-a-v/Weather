#include "SensorValues.h"

int SensorValues::GetSensorIndex(SensorId id) {
    return sensorIndex[id];
}

SensorValues::SensorValues() {
    sensorIndex[RadioOutdoor] = 0;
    sensorIndex[RadioBoiler] = 1;
    sensorIndex[RadioBedroom] = 2;
    sensorIndex[RadioLounge] = 3;
    sensorIndex[RadioMansard] = 4;
    sensorIndex[RadioStudy] = 5;
    sensorIndex[DirectBoiler] = 6;
    sensorIndex[DirectIndoor] = 7;
    sensorIndex[DirectOtdoor] = 8;
    sensorIndex[GlobalSun] = 9;
    sensorIndex[GlobalWind] = 10;
    sensorIndex[GlobalOutdoor] = 11;

    for(int i = 0; i < SENSORS_COUNT; i++) {
        lastSensorValues[i] = DEFAULT_TEMPERATURE;
        avgSensorValues[i] = DEFAULT_TEMPERATURE;
        lastSensorResponseTime[i] = DEFAULT_TIME;
        firstSensorResponseTime[i] = DEFAULT_TIME;
    }
}

SensorValues::~SensorValues() {
}

float SensorValues::GetAverageSensorValue(SensorId id) {
    int sensorIndex = GetSensorIndex(id);
    return (lastSensorResponseTime[sensorIndex] == firstSensorResponseTime[sensorIndex]) ?
        lastSensorValues[sensorIndex] : avgSensorValues[sensorIndex] /
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
    if(avgSensorValues[sensorIndex] == DEFAULT_TEMPERATURE) {
        firstSensorResponseTime[sensorIndex] = time;
        avgSensorValues[sensorIndex] = value;
    } else {
        avgSensorValues[sensorIndex] += (time - lastSensorResponseTime[sensorIndex]) * value;
    }
    lastSensorResponseTime[sensorIndex] = time;
    lastSensorValues[sensorIndex] = value;
}

std::string SensorValues::ToString(SensorId id) {
    float lastValue = GetLastSensorValue(id);
    if(lastValue == DEFAULT_TEMPERATURE) {
        return "---";
    } else {
        std::stringstream ss;
        ss << lastValue << " (" << Utils::FormatTime(GetLastSensorResponseTime(id)) << ")"
            << " [" << GetAverageSensorValue(id) << "]";
        return ss.str();
    }
}

std::string SensorValues::GetSensorColumns() {
    std::stringstream ss;
    ss << "`AvgRadioOutdoor`, `LastRadioOutdoor`" <<
        ", `AvgRadioBoiler`, `LastRadioBoiler`" <<
        ", `AvgRadioBedroom`, `LastRadioBedroom`" <<
        ", `AvgRadioLounge`, `LastRadioLounge`" <<
        ", `AvgRadioMansard`, `LastRadioMansard`" <<
        ", `AvgRadioStudy`, `LastRadioStudy`" <<
        ", `AvgDirectBoiler`, `LastDirectBoiler`" <<
        ", `AvgDirectIndoor`, `LastDirectIndoor`" <<
        ", `AvgDirectOtdoor`, `LastDirectOtdoor`" <<
        ", `GlobalSun`" <<
        ", `GlobalWind`" <<
        ", `GlobalOutdoor`";
    return ss.str();
}

std::string SensorValues::GetSensorValues() {
    std::stringstream ss;
    ss << GetAverageSensorValue(RadioOutdoor) << ", " <<
        GetLastSensorValue(RadioOutdoor) << ", " <<
        GetAverageSensorValue(RadioBoiler) << ", " <<
        GetAverageSensorValue(RadioBoiler) << ", " <<
        GetAverageSensorValue(RadioBedroom) << ", " <<
        GetAverageSensorValue(RadioBedroom) << ", " <<
        GetAverageSensorValue(RadioLounge) << ", " <<
        GetAverageSensorValue(RadioLounge) << ", " <<
        GetAverageSensorValue(RadioMansard) << ", " <<
        GetAverageSensorValue(RadioMansard) << ", " <<
        GetAverageSensorValue(RadioStudy) << ", " <<
        GetAverageSensorValue(RadioStudy) << ", " <<
        GetAverageSensorValue(DirectBoiler) << ", " <<
        GetAverageSensorValue(DirectBoiler) << ", " <<
        GetAverageSensorValue(DirectIndoor) << ", " <<
        GetAverageSensorValue(DirectIndoor) << ", " <<
        GetAverageSensorValue(DirectOtdoor) << ", " <<
        GetAverageSensorValue(DirectOtdoor) << ", " <<
        GetAverageSensorValue(GlobalSun) << ", " <<
        GetAverageSensorValue(GlobalWind) << ", " <<
        GetAverageSensorValue(GlobalOutdoor);
    return ss.str();
}
