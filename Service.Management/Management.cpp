#include "Management.h"

Management::Management(Storage *storage, GlobalWeather *globalWeatherSystem) {
    this->storage = storage;
    this->globalWeatherSystem = globalWeatherSystem;
    this->controlTable = storage->ReadControlTable();
    this->settingsTable = storage->ReadSettingsTable();
    auto min_max_indoor = std::minmax_element(this->controlTable->begin(), this->controlTable->end(),
                                              [](const ControlValue &c1, const ControlValue &c2) -> bool { return c1.Indoor < c2.Indoor; });
    auto min_max_outdoor = std::minmax_element(this->controlTable->begin(), this->controlTable->end(),
                                               [](const ControlValue &c1, const ControlValue &c2) -> bool { return c1.Outdoor < c2.Outdoor; });
    this->minIndoorTemperature = min_max_indoor.first->Indoor;
    this->maxIndoorTemperature = min_max_indoor.second->Indoor;
    this->minOutdoorTemperature = min_max_outdoor.first->Outdoor;
    this->maxOutdoorTemperature = min_max_outdoor.second->Outdoor;
    this->sensorValues = new SensorValues();
    this->cycleInfo = new CycleInfo(false, DEFAULT_TEMPERATURE, DEFAULT_TEMPERATURE, Utils::GetTime(), Utils::GetTime());
    sprintf(additionalInfo, "");
#ifndef TEST
    Utils::SetupGPIO();
    ReadTemplate();
    StoreGlobalWeather();
#endif
}
void Management::ProcessResponce(const DeviceResponce& responce) {
    std::lock_guard<std::mutex> lock(management_lock);
    time_t now = Utils::GetTime();
    sensorValues->AddSensorValue(responce.Sensor, responce.Value, now);
    if(responce.Sensor == boilerSensorId) {
        cycleInfo->ProcessBoilerTemperature(responce.Value, now);
        Utils::SetGPIOValues(BOILER_STATUS_PIN, !cycleInfo->IsBoilerOn());
    }
    if(cycleInfo->IsCycleEnd() && CanStartNewCycle()) {
        BeginNewCycle(now);
    }
    TemplateUtils::WriteCurrentStatus(sensorValues, cycleInfo, statusTemplate, additionalInfo, now);
}
void Management::BeginNewCycle(const time_t &now) {
    CycleStatictics *lastCycleStat = cycleInfo->GetStatictics();
    SensorValues *lastSensorValues = sensorValues;
    storage->SaveCycleStatistics(lastCycleStat, lastSensorValues);

    sensorValues = new SensorValues();
    StoreGlobalWeather();
    SetBoilerSensorId(lastSensorValues);
    float boilerTemperature = GetBoilerTemperature(lastSensorValues);
    time_t boilerResponseTime = GetLastBoilerResponseTime(lastSensorValues);
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
    cycleInfo = new CycleInfo(newCycleWillHeating, adjustBoilerTemperature, boilerTemperature, boilerResponseTime, now);
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
    time_t now = Utils::GetTime();
    CurrentWeather *weather = globalWeatherSystem->GetWeather();
    sensorValues->AddSensorValue(GlobalSun, weather->GetSun(), now);
    sensorValues->AddSensorValue(GlobalWind, weather->GetWind(), now);
    sensorValues->AddSensorValue(GlobalOutdoor, weather->GetTemperature(), now);
    delete weather;
    weather = nullptr;
}
void Management::SetBoilerSensorId(SensorValues * sensorValues) {
    boilerSensorId = (sensorValues->GetLastSensorResponseTime(DirectBoiler) != DEFAULT_TIME) ? DirectBoiler : RadioBoiler;
}
bool Management::CanStartNewCycle() {
    return GetIndoorTemperature(sensorValues) != DEFAULT_TEMPERATURE &&
        GetOutdoorTemperature(sensorValues) != DEFAULT_TEMPERATURE &&
        GetBoilerTemperature(sensorValues) != DEFAULT_TEMPERATURE;
}
float Management::GetBoilerTemperature(SensorValues *sensorValues) {
    /* RadioBoiler, DirectBoiler */
    return (sensorValues->GetLastSensorResponseTime(DirectBoiler) != DEFAULT_TIME) ?
        sensorValues->GetLastSensorValue(DirectBoiler) :
        sensorValues->GetLastSensorValue(RadioBoiler);
}
time_t Management::GetLastBoilerResponseTime(SensorValues * sensorValues) {
    return (sensorValues->GetLastSensorResponseTime(DirectBoiler) != DEFAULT_TIME) ?
        sensorValues->GetLastSensorResponseTime(DirectBoiler) :
        sensorValues->GetLastSensorResponseTime(RadioBoiler);
}
float Management::GetIndoorTemperature(SensorValues *sensorValues) {
    /* RadioBedroom, RadioLounge, RadioMansard, RadioStudy, DirectIndoor */
    float avg = 0;
    int count = 0;
    if(sensorValues->GetLastSensorResponseTime(RadioBedroom) != DEFAULT_TIME) {
        avg += sensorValues->GetLastSensorValue(RadioBedroom);
        count++;
    }
    if(sensorValues->GetLastSensorResponseTime(RadioLounge) != DEFAULT_TIME) {
        avg += sensorValues->GetLastSensorValue(RadioLounge);
        count++;
    }
    if(sensorValues->GetLastSensorResponseTime(RadioMansard) != DEFAULT_TIME) {
        avg += sensorValues->GetLastSensorValue(RadioMansard);
        count++;
    }
    if(sensorValues->GetLastSensorResponseTime(RadioStudy) != DEFAULT_TIME) {
        avg += sensorValues->GetLastSensorValue(RadioStudy);
        count++;
    }
    if(sensorValues->GetLastSensorResponseTime(DirectIndoor) != DEFAULT_TIME) {
        avg += sensorValues->GetLastSensorValue(DirectIndoor);
        count++;
    }
    return count != 0 ? (avg / count) : DEFAULT_TEMPERATURE;
}
float Management::GetOutdoorTemperature(SensorValues *sensorValues) {
    /* RadioOutdoor, DirectOtdoor, GlobalOutdoor */
    return (sensorValues->GetLastSensorResponseTime(DirectOtdoor) != DEFAULT_TIME) ?
        sensorValues->GetAverageSensorValue(DirectOtdoor) :
        ((sensorValues->GetLastSensorResponseTime(RadioOutdoor) != DEFAULT_TIME) ?
         sensorValues->GetAverageSensorValue(RadioOutdoor) : sensorValues->GetAverageSensorValue(GlobalOutdoor));
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
