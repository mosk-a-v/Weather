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
    this->cycleInfo = new CycleInfo(false, DEFAULT_TEMPERATURE, DEFAULT_TEMPERATURE, GetTime());
#ifndef TEST
    SetupGPIO();
    ReadTemplate();
    StoreGlobalWeather();
#endif
}
void Management::ProcessResponce(const DeviceResponce& responce) {
    std::lock_guard<std::mutex> lock(mu);
    time_t now = GetTime();
    sensorValues->AddSensorValue(responce.Sensor, responce.Value, now);
    if(responce.Sensor == GetBoilerSensorId()) {
        cycleInfo->ProcessBoilerTemperature(responce.Value, now);
    }
    if(cycleInfo->IsCycleEnd()) {
        BeginNewCycle(now);
    }
    SetGPIOValues();
    TemplateUtils::WriteCurrentStatus(sensorValues, cycleInfo, statusTemplate, now);
}
void Management::BeginNewCycle(const time_t &now) {
    CycleStatictics *lastCycleStat = cycleInfo->GetStatictics();
    SensorValues *lastSensorValues = sensorValues;
    sensorValues = new SensorValues();
    StoreGlobalWeather();
    float requiredIndoorTemperature = GetRequiredIndoorTemperature();
    float requiredBoilerTemperature = GetRequiredBoilerTemperature(sensorValues->GetLastSensorValue(GlobalSun), sensorValues->GetLastSensorValue(GlobalWind), GetAvgOutdoorTemperature(), requiredIndoorTemperature);
    float adjustBoilerTemperature = Utils::GetAdjustBoilerTemperature(GetAvgIndoorTemperature(), requiredIndoorTemperature, requiredBoilerTemperature);
    float boilerTemperature = lastSensorValues->GetLastSensorValue(GetBoilerSensorId());
    bool newCycleWillHeating = boilerTemperature <= adjustBoilerTemperature;
    delete cycleInfo;
    cycleInfo = new CycleInfo(newCycleWillHeating, adjustBoilerTemperature, boilerTemperature, now);
    storage->SaveCycleStatistics(lastCycleStat, lastSensorValues);
    delete lastCycleStat;
    delete lastSensorValues;
}
float Management::GetRequiredIndoorTemperature() {
    const long TwoHourSeconds = 60 * 60 * 2;
    std::time_t t = GetTime() + TwoHourSeconds;
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
std::time_t Management::GetTime() {
    return std::time(0);
}
void Management::SetupGPIO() {
    wiringPiSetupGpio();
    pinMode(PIN, OUTPUT);
    digitalWrite(PIN, true);
}
void Management::SetGPIOValues() {
    bool pinValue = !cycleInfo->IsBoilerOn();
    digitalWrite(PIN, pinValue);
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
    time_t now = GetTime();
    CurrentWeather *weather;
    weather = globalWeatherSystem->GetWeather();
    sensorValues->AddSensorValue(GlobalSun, weather->GetSun(), now);
    sensorValues->AddSensorValue(GlobalWind, weather->GetWind(), now);
    delete weather;
}

SensorId Management::GetBoilerSensorId() {
    return RadioBoiler;
}

float Management::GetAvgIndoorTemperature() {
    return 0.0f;
}

float Management::GetAvgOutdoorTemperature() {
    return 0.0f;
}

Management::~Management() {
    delete settingsTable;
    delete controlTable;
}
