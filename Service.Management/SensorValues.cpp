#include "SensorValues.h"

int SensorValues::GetSensorIndex(SensorId id) {
    return sensorIndex[id];
}

SensorValues::SensorValues() {
    sensorIndex[Undefined] = 0;
    sensorIndex[RadioOutdoor] = 1;
    sensorIndex[RadioBoiler] = 2;
    sensorIndex[RadioBedroom] = 3;
    sensorIndex[RadioLounge] = 4;
    sensorIndex[RadioMansard] = 5;
    sensorIndex[RadioStudy] = 6;
    sensorIndex[DirectBoiler] = 7;
    sensorIndex[DirectIndoor] = 8;
    sensorIndex[DirectOtdoor] = 9;
    sensorIndex[GlobalSun] = 10;
    sensorIndex[GlobalWind] = 11;
    sensorIndex[GlobalOutdoor] = 12;
    sensorIndex[RadioLoungeHumidity] = 13;
    sensorIndex[RadioMansardHumidity] = 14;
    sensorIndex[RadioStudyHumidity] = 15;
    sensorIndex[RadioKitchen] = 16;
    sensorIndex[RadioKitchenHumidity] = 17;
    sensorIndex[ThermocoupleVoltage] = 18;

    for(int i = 0; i < SENSORS_COUNT; i++) {
        lastSensorValues[i] = DEFAULT_TEMPERATURE;
        avgSensorValues[i] = DEFAULT_TEMPERATURE;
        minSensorValues[i] = DEFAULT_TEMPERATURE;
        maxSensorValues[i] = DEFAULT_TEMPERATURE;
        lastSensorResponseTime[i] = DEFAULT_TIME;
        firstSensorResponseTime[i] = DEFAULT_TIME;
        sensorWarnings[i] = false;
    }
}

SensorValues::~SensorValues() {}

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

float SensorValues::GetMinSensorValue(SensorId id) {
    int sensorIndex = GetSensorIndex(id);
    return minSensorValues[sensorIndex];
}

float SensorValues::GetMaxSensorValue(SensorId id) {
    int sensorIndex = GetSensorIndex(id);
    return maxSensorValues[sensorIndex];
}

bool SensorValues::IsSensorWarning(SensorId id) {
    int sensorIndex = GetSensorIndex(id);
    return sensorWarnings[sensorIndex];
}

time_t SensorValues::GetLastSensorResponseTime(SensorId id) {
    int sensorIndex = GetSensorIndex(id);
    return lastSensorResponseTime[sensorIndex];
}

void SensorValues::AddSensorValue(SensorId id, float value, bool warning, time_t time) {
    int sensorIndex = GetSensorIndex(id);
    if(value == DEFAULT_TEMPERATURE) {
        return;
    }
    if(avgSensorValues[sensorIndex] == DEFAULT_TEMPERATURE) {
        firstSensorResponseTime[sensorIndex] = time;
        avgSensorValues[sensorIndex] = value;
        minSensorValues[sensorIndex] = value;
        maxSensorValues[sensorIndex] = value;
    } else {
        avgSensorValues[sensorIndex] += (time - lastSensorResponseTime[sensorIndex]) * value;
    }
    if(lastSensorValues[sensorIndex] != DEFAULT_TEMPERATURE && fabs(lastSensorValues[sensorIndex] - value) > MAX_SENSOR_DEVIATION) {
        sensorWarnings[sensorIndex] = true;
    }
    lastSensorResponseTime[sensorIndex] = time;
    lastSensorValues[sensorIndex] = value;
    if(!sensorWarnings[sensorIndex]) {
        sensorWarnings[sensorIndex] = warning;
    }
    if(minSensorValues[sensorIndex] > value) {
        minSensorValues[sensorIndex] = value;
    }
    if(maxSensorValues[sensorIndex] < value) {
        maxSensorValues[sensorIndex] = value;
    }
}

std::string SensorValues::ToString(SensorId id) {
    float lastValue = GetLastSensorValue(id);
    if(lastValue == DEFAULT_TEMPERATURE) {
        return "---";
    } else {
        std::stringstream ss;
        ss << std::fixed;
        ss.precision(1);
        ss << lastValue;
        if(IsSensorWarning(id)) {
            ss << " ! ";
        }
        ss << " (" << Utils::FormatTime(GetLastSensorResponseTime(id)) << ")"
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
        ", `GlobalOutdoor`" <<
        ", `LoungeHumidity`" <<
        ", `MansardHumidity`" <<
        ", `StudyHumidity`" <<
        ", `AvgRadioKitchen`, `LastRadioKitchen`" <<
        ", `KitchenHumidity`";
    return ss.str();
}

std::string SensorValues::GetSensorValues() {
    std::stringstream ss;
    ss << GetAverageSensorValue(RadioOutdoor) << ", " <<
        GetLastSensorValue(RadioOutdoor) << ", " <<
        GetAverageSensorValue(RadioBoiler) << ", " <<
        GetLastSensorValue(RadioBoiler) << ", " <<
        GetAverageSensorValue(RadioBedroom) << ", " <<
        GetLastSensorValue(RadioBedroom) << ", " <<
        GetAverageSensorValue(RadioLounge) << ", " <<
        GetLastSensorValue(RadioLounge) << ", " <<
        GetAverageSensorValue(RadioMansard) << ", " <<
        GetLastSensorValue(RadioMansard) << ", " <<
        GetAverageSensorValue(RadioStudy) << ", " <<
        GetLastSensorValue(RadioStudy) << ", " <<
        GetAverageSensorValue(DirectBoiler) << ", " <<
        GetLastSensorValue(DirectBoiler) << ", " <<
        GetAverageSensorValue(DirectIndoor) << ", " <<
        GetLastSensorValue(DirectIndoor) << ", " <<
        GetAverageSensorValue(DirectOtdoor) << ", " <<
        GetLastSensorValue(DirectOtdoor) << ", " <<
        GetAverageSensorValue(GlobalSun) << ", " <<
        GetAverageSensorValue(GlobalWind) << ", " <<
        GetAverageSensorValue(GlobalOutdoor) << ", " <<
        GetAverageSensorValue(RadioLoungeHumidity) << ", " <<
        GetAverageSensorValue(RadioMansardHumidity) << ", " <<
        GetAverageSensorValue(RadioStudyHumidity) << ", " <<
        GetAverageSensorValue(RadioKitchen) << ", " <<
        GetLastSensorValue(RadioKitchen) << ", " <<
        GetAverageSensorValue(RadioKitchenHumidity);
    return ss.str();
}

nlohmann::json SensorValues::ToJson() {
    nlohmann::json result;
    for(auto it = sensorIndex.begin(); it != sensorIndex.end(); it++) {
        if(((SensorId)it->first) == Undefined) {
            continue;
        }
        nlohmann::json value;
        value["SensorId"] = it->first;
        value["Last"] = lastSensorValues[it->second];
        value["Average"] = GetAverageSensorValue((SensorId)it->first);
        value["Warning"] = sensorWarnings[it->second];
        value["Time"] = lastSensorResponseTime[it->second];
        value["IsInvalid"] = lastSensorValues[it->second] == DEFAULT_TEMPERATURE;
        result.push_back(value);
    }
    return result;
}
