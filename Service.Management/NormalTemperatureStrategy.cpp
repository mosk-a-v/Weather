#include "NormalTemperatureStrategy.h"

NormalTemperatureStrategy::NormalTemperatureStrategy() : TemperatureStrategyBase() {
    LoadSettingsTable();
    LoadControlTable();
}
NormalTemperatureStrategy::~NormalTemperatureStrategy() {
    if(settingsTable != nullptr) {
        delete settingsTable;
        settingsTable = nullptr;
    }
    if(controlTable != nullptr) {
        delete controlTable;
        controlTable = nullptr;
    }
}
float NormalTemperatureStrategy::GetBoilerTemperature(SensorValues *lastSensorValues, float sun, float wind) {
    float outdoorTemperature = GetOutdoorTemperature(lastSensorValues);
    float indoorTemperature = GetIndoorTemperature(lastSensorValues);

    float requiredIndoorTemperature = GetRequiredIndoorTemperature();
    float requiredBoilerTemperature = GetRequiredBoilerTemperature(sun, wind, outdoorTemperature, requiredIndoorTemperature);
    return Utils::GetAdjustBoilerTemperature(indoorTemperature, requiredIndoorTemperature, requiredBoilerTemperature);
}
void NormalTemperatureStrategy::LoadControlTable() {
    if(controlTable != nullptr) {
        delete controlTable;
        controlTable = nullptr;
    }
    controlTable = Storage::ReadControlTable();
    auto min_max_indoor = std::minmax_element(controlTable->begin(), controlTable->end(),
                                              [](const ControlValue &c1, const ControlValue &c2) -> bool { return c1.Indoor < c2.Indoor; });
    auto min_max_outdoor = std::minmax_element(controlTable->begin(), controlTable->end(),
                                               [](const ControlValue &c1, const ControlValue &c2) -> bool { return c1.Outdoor < c2.Outdoor; });
    minIndoorTemperature = min_max_indoor.first->Indoor;
    maxIndoorTemperature = min_max_indoor.second->Indoor;
    minOutdoorTemperature = min_max_outdoor.first->Outdoor;
    maxOutdoorTemperature = min_max_outdoor.second->Outdoor;
}
void NormalTemperatureStrategy::LoadSettingsTable() {
    if(settingsTable != nullptr) {
        delete settingsTable;
        settingsTable = nullptr;
    }
    settingsTable = Storage::ReadSettingsTable();
}
float NormalTemperatureStrategy::GetRequiredIndoorTemperature() {
    const long TwoHourSeconds = 60 * 60 * 2;
    std::time_t t = Utils::GetTime() + TwoHourSeconds;
    std::tm *requiredDateTime = std::localtime(&t);

    std::vector<SettingValue>::iterator result = std::find_if(settingsTable->begin(), settingsTable->end(),
                                                              [requiredDateTime](const SettingValue& s) -> bool {
        return s.Hour == requiredDateTime->tm_hour && s.WeekDay == ((requiredDateTime->tm_wday + 6) % 7 + 1);
    });
    if(result == settingsTable->end()) {
        std::stringstream ss;
        ss << "Hour: " << requiredDateTime->tm_hour << "; WeekDay:" << requiredDateTime->tm_wday;
        Utils::WriteLogInfo(LOG_ERR, "Wrong Now.", ss.str());
        return 20;
    } else {
        return result->Temperature;
    }
}
float NormalTemperatureStrategy::GetRequiredBoilerTemperature(int sun, int wind, float outdoorTemperature, float indoorTemperature) {
    float i1 = std::max(minIndoorTemperature, round((indoorTemperature - 0.9999f) / 2.0f) * 2.0f);
    float i2 = std::min(maxIndoorTemperature, round((indoorTemperature + 0.9999f) / 2.0f) * 2.0f);
    if(i1 == i2) {
        if(i1 == minIndoorTemperature) {
            i2 = i1 + 2;
        } else {
            i1 = i2 - 2;
        }
    }
    float o1 = std::max(minOutdoorTemperature, round((outdoorTemperature - 4.9999f) / 10.0f) * 10.0f);
    float o2 = std::min(maxOutdoorTemperature, round((outdoorTemperature + 4.9999f) / 10.0f) * 10.0f);
    if(o1 == o2) {
        if(o1 == minOutdoorTemperature) {
            o2 = o1 + 10;
        } else {
            o1 = o2 - 10;
        }
    }

    float f11 = GetControlValue(0, 0, o1, i1);
    float f12 = GetControlValue(0, 0, o1, i2);
    float f21 = GetControlValue(0, 0, o2, i1);
    float f22 = GetControlValue(0, 0, o2, i2);
    float A = (o2 - outdoorTemperature) / (o2 - o1);
    float B = (outdoorTemperature - o1) / (o2 - o1);
    float C = (i2 - indoorTemperature) / (i2 - i1);
    float D = (indoorTemperature - i1) / (i2 - i1);
    float fR1 = A * f11 + B * f21;
    float fR2 = A * f12 + B * f22;
    float t = C * fR1 + D * fR2;
    float sa = Utils::GetSunAdjust(sun);
    float wa = Utils::GetWindAdjust(wind);
    return t + wa - sa;
}
float NormalTemperatureStrategy::GetControlValue(int sun, int wind, float outdoorTemperature, float indoorTemperature) {
    if(outdoorTemperature > maxOutdoorTemperature) {
        outdoorTemperature = maxOutdoorTemperature;
    }
    if(outdoorTemperature < minOutdoorTemperature) {
        outdoorTemperature = minOutdoorTemperature;
    }
    if(indoorTemperature > maxIndoorTemperature) {
        indoorTemperature = maxIndoorTemperature;
    }
    if(indoorTemperature < minIndoorTemperature) {
        indoorTemperature = minIndoorTemperature;
    }
    auto result = std::find_if(controlTable->begin(), controlTable->end(),
                               [sun, wind, outdoorTemperature, indoorTemperature](const ControlValue& c) -> bool {
        return c.Sun == sun && c.Wind == wind && c.Outdoor == outdoorTemperature && c.Indoor == indoorTemperature; });
    if(result == controlTable->end()) {
        std::stringstream ss;
        ss << "Sun: " << sun << "; Wind:" << wind << "; Outdoor:" << outdoorTemperature << "; Indoor:" << indoorTemperature;
        Utils::WriteLogInfo(LOG_ERR, "Wrong Temperature. ", ss.str());
        return 40;
    } else {
        return result->Boiler;
    }
}
SensorId NormalTemperatureStrategy::GetBoilerSensorId(SensorValues * sensorValues) {
    if(sensorValues->GetLastSensorResponseTime(DirectBoiler) != DEFAULT_TIME) {
        return DirectBoiler;
    } else if(sensorValues->GetLastSensorResponseTime(RadioBoiler) != DEFAULT_TIME) {
        return RadioBoiler;
    }
    return Undefined;
}