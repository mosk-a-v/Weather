#include "Management.h"

Management::Management(Storage *storage, GlobalWeather *globalWeatherSystem, std::map<std::string, SensorInfo> *sensorsTable) {
    this->storage = storage;
    this->globalWeatherSystem = globalWeatherSystem;
    this->sensorsTable = sensorsTable;
    LoadSettingsTable();
    LoadControlTable();
    this->sensorValues = new SensorValues();
    this->cycleInfo = new CycleInfo(Utils::GetTime());
    sprintf(additionalInfo, "");
#ifndef TEST
    Utils::SetupGPIO();
    ReadTemplate();
    StoreGlobalWeather();
#endif
}
void Management::LoadControlTable() {
    if(controlTable != nullptr) {
        delete controlTable;
        controlTable = nullptr;
    }
    controlTable = storage->ReadControlTable();
    auto min_max_indoor = std::minmax_element(controlTable->begin(), controlTable->end(),
                                              [](const ControlValue &c1, const ControlValue &c2) -> bool { return c1.Indoor < c2.Indoor; });
    auto min_max_outdoor = std::minmax_element(controlTable->begin(), controlTable->end(),
                                               [](const ControlValue &c1, const ControlValue &c2) -> bool { return c1.Outdoor < c2.Outdoor; });
    minIndoorTemperature = min_max_indoor.first->Indoor;
    maxIndoorTemperature = min_max_indoor.second->Indoor;
    minOutdoorTemperature = min_max_outdoor.first->Outdoor;
    maxOutdoorTemperature = min_max_outdoor.second->Outdoor;
}
void Management::LoadSettingsTable() {
    if(settingsTable != nullptr) {
        delete settingsTable;
        settingsTable = nullptr;
    }
    settingsTable = storage->ReadSettingsTable();
}
void Management::ProcessResponce(const DeviceResponce& responce) {
    std::lock_guard<std::mutex> lock(management_lock);
    time_t now = Utils::GetTime();
    sensorValues->AddSensorValue(responce.Sensor, responce.Value, !responce.Battery, now);
    if(responce.Humidity > 0) {
        sensorValues->AddSensorValue((SensorId)((int)responce.Sensor + 20), responce.Humidity, !responce.Battery, now);
    }
    if(responce.Sensor == boilerSensorId) {
        cycleInfo->ProcessBoilerTemperature(responce.Value, now);
    } else if(sensorValues->GetLastSensorResponseTime(boilerSensorId) == DEFAULT_TIME &&
              cycleInfo->GetCycleLength(now) > SENSOR_TIMEOUT) {
        cycleInfo->EndCycle(SensorError, now);
    }
    if(cycleInfo->IsCycleEnd() && GetIndoorTemperature(sensorValues) != DEFAULT_TEMPERATURE) {
        BeginNewCycle(now);
    }
    Utils::SetGPIOValues(BOILER_STATUS_PIN, !cycleInfo->IsBoilerOn());
    TemplateUtils::WriteCurrentStatus(sensorValues, cycleInfo, statusTemplate, additionalInfo, now);
}
void Management::BeginNewCycle(const time_t &now) {
    CycleStatictics *lastCycleStat = cycleInfo->GetStatictics();
    SensorValues *lastSensorValues = sensorValues;
    storage->SaveCycleStatistics(lastCycleStat, lastSensorValues);

    sensorValues = new SensorValues();
    StoreGlobalWeather();
    if(SetBoilerSensorId(lastSensorValues)) {
        float boilerTemperature = lastSensorValues->GetLastSensorValue(boilerSensorId);
        time_t boilerResponseTime = lastSensorValues->GetLastSensorResponseTime(boilerSensorId);
        float outdoorTemperature = GetOutdoorTemperature(lastSensorValues);
        float indoorTemperature = GetIndoorTemperature(lastSensorValues);
        float sun = sensorValues->GetLastSensorValue(GlobalSun);
        float wind = sensorValues->GetLastSensorValue(GlobalWind);

        float requiredIndoorTemperature = GetRequiredIndoorTemperature();
        float requiredBoilerTemperature = GetRequiredBoilerTemperature(sun, wind, outdoorTemperature, requiredIndoorTemperature);
        float adjustBoilerTemperature = Utils::GetAdjustBoilerTemperature(indoorTemperature, requiredIndoorTemperature, requiredBoilerTemperature);
        bool newCycleWillHeating = boilerTemperature <= adjustBoilerTemperature;
        sprintf(additionalInfo, "Boiler: %.2f; Outdoor: %.2f; Indoor: %.2f", boilerTemperature, outdoorTemperature, indoorTemperature);

        delete cycleInfo;
        cycleInfo = new CycleInfo(newCycleWillHeating, adjustBoilerTemperature, boilerTemperature, boilerResponseTime, DEFAULT_LATENCY, now);
    } else {
        sprintf(additionalInfo, "Sensor error");
        delete cycleInfo;
        cycleInfo = new CycleInfo(now);
    }
    delete lastCycleStat;
    lastCycleStat = nullptr;
    delete lastSensorValues;
    lastSensorValues = nullptr;
}
float Management::GetRequiredIndoorTemperature() {
    const long TwoHourSeconds = 60 * 60 * 2;
    std::time_t t = Utils::GetTime() + TwoHourSeconds;
    std::tm *requiredDateTime = std::localtime(&t);

    std::vector<SettingValue>::iterator result = std::find_if(settingsTable->begin(), settingsTable->end(),
                                                              [requiredDateTime](const SettingValue& s) -> bool {
        return s.Hour == requiredDateTime->tm_hour && s.WeekDay == ((requiredDateTime->tm_wday + 6) % 7 + 1);
    });
    if(result == settingsTable->end()) {
        std::stringstream message_stream;
        message_stream << "Wrong Now. Hour: " << requiredDateTime->tm_hour << "; WeekDay:" << requiredDateTime->tm_wday << std::endl;
        sd_journal_print(LOG_INFO, message_stream.str().c_str());
        return 20;
    } else {
        return result->Temperature;
    }
}
float Management::GetRequiredBoilerTemperature(int sun, int wind, float outdoorTemperature, float indoorTemperature) {
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
float Management::GetControlValue(int sun, int wind, float outdoorTemperature, float indoorTemperature) {
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
        std::stringstream message_stream;
        message_stream << "Wrong Temperature. Sun: " << sun << "; Wind:" << wind << "; Outdoor:" << outdoorTemperature << "; Indoor:" << indoorTemperature;
        sd_journal_print(LOG_INFO, message_stream.str().c_str());
        return 40;
    } else {
        return result->Boiler;
    }
}

void Management::ReadTemplate() {
    try {
        std::ifstream templateStream;
        templateStream.open(TEMPLATE_FILE_NAME);
        std::stringstream buffer;
        buffer << templateStream.rdbuf();
        statusTemplate = buffer.str();
        templateStream.close();
    } catch(const std::exception &e) {
        statusTemplate = "";
        std::stringstream ss;
        ss << "Template read exception." << e.what();
        sd_journal_print(LOG_ERR, ss.str().c_str());
    }
}
void Management::StoreGlobalWeather() {
    CurrentWeather *weather = globalWeatherSystem->GetWeather();
    sensorValues->AddSensorValue(GlobalSun, weather->GetSun(), false, weather->LastUpdateTime);
    sensorValues->AddSensorValue(GlobalWind, weather->GetWind(), false, weather->LastUpdateTime);
    sensorValues->AddSensorValue(GlobalOutdoor, weather->GetTemperature(), false, weather->LastUpdateTime);
    delete weather;
    weather = nullptr;
}
bool Management::SetBoilerSensorId(SensorValues * sensorValues) {
    if(sensorValues->GetLastSensorResponseTime(DirectBoiler) != DEFAULT_TIME) {
        boilerSensorId = DirectBoiler;
        return true;
    } else if(sensorValues->GetLastSensorResponseTime(RadioBoiler) != DEFAULT_TIME) {
        boilerSensorId = RadioBoiler;
        return true;
    }
    boilerSensorId = Undefined;
    return false;
}
float Management::GetIndoorTemperature(SensorValues *sensorValues) {
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
float Management::GetOutdoorTemperature(SensorValues *sensorValues) {
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
Management::~Management() {
    if(settingsTable != nullptr) {
        delete settingsTable;
        settingsTable = nullptr;
    }
    if(controlTable != nullptr) {
        delete controlTable;
        controlTable = nullptr;
    }
    if(cycleInfo != nullptr) {
        delete cycleInfo;
        cycleInfo = nullptr;
    }
    if(sensorValues != nullptr) {
        delete sensorValues;
        sensorValues = nullptr;
    }
    globalWeatherSystem = nullptr;
    storage = nullptr;
}
