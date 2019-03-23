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
#ifndef TEST
    SetupGPIO();
    ReadTemplate();
#endif
    CurrentWeather weather;
    globalWeatherSystem->GetWeather(weather);
    cycleInfo = new CycleInfo(false, DEFAULT_TEMPERATURE, weather, GetTime(), statusTemplate, "Starting");
}
void Management::ProcessResponce(const DeviceResponce& responce) {
    time_t now = GetTime();
    switch(responce.Sensor) {
        case Boiler:
            cycleInfo->AddBoilerTemperatue(responce.Value, now);
            break;
        case Indoor:
            cycleInfo->AddIndoorTemperature(responce.Value, now);
            break;
        case Outdoor:
            cycleInfo->AddOutdoorTemperature(responce.Value, now);
            break;
        default:
            break;
    }
    ManageBoiler(responce.Value, now);
}
void Management::ManageBoiler(float actualilerTemperature, std::time_t now) {
    if(cycleInfo->IsCycleEnd()) {
        BeginNewCycle(now);
    }
    SetGPIOValues();
}
void Management::BeginNewCycle(const time_t &now) {
    CurrentWeather weather;
    globalWeatherSystem->GetWeather(weather);
    CycleStatictics *lastCycleStat = cycleInfo->GetStatictics();
    stringstream additionalInfoStream;
    additionalInfoStream << fixed;
    additionalInfoStream.precision(1);
    additionalInfoStream << "Ветер: " << weather.GetWind() << "; Солнце: " << weather.GetSun() <<
        ". Прошлый цикл (" << lastCycleStat->CycleLength << "c, " << (lastCycleStat->IsHeating ? "нагрев" : "охлаждение") <<
        ",  " << lastCycleStat->AvgIndoor << ", " << lastCycleStat->AvgOutdoor << ", " << lastCycleStat->AvgBoiler <<
        //", " << requiredBoilerTemperature << ", " << adjustBoilerTemperature <<
        ", " << lastCycleStat->BoilerRequired << ").";
    delete cycleInfo;
    float requiredIndoorTemperature = GetRequiredIndoorTemperature();
    float requiredBoilerTemperature = GetRequiredBoilerTemperature(weather.GetSun(), weather.GetWind(), lastCycleStat->AvgOutdoor, requiredIndoorTemperature);
    float adjustBoilerTemperature = GetAdjustBoilerTemperature(lastCycleStat->AvgIndoor, requiredIndoorTemperature, requiredBoilerTemperature);
    if(lastCycleStat->BoilerRequired != DEFAULT_TEMPERATURE) {
        adjustBoilerTemperature += (lastCycleStat->BoilerRequired - lastCycleStat->AvgBoiler);
    }
    adjustBoilerTemperature -= 3; //ToDo
    bool newCycleWillHeating = lastCycleStat->LastBoiler <= adjustBoilerTemperature;
    cycleInfo = new CycleInfo(newCycleWillHeating, adjustBoilerTemperature, weather, now, statusTemplate, additionalInfoStream.str());
    cycleInfo->AddIndoorTemperature(lastCycleStat->LastIndoor, now);
    cycleInfo->AddOutdoorTemperature(lastCycleStat->LastOutdoor, now);
    cycleInfo->AddBoilerTemperatue(lastCycleStat->LastBoiler, now);
    storage->SaveCycleStatistics(lastCycleStat);
    delete lastCycleStat;
}
float Management::GetAdjustBoilerTemperature(float indoorTemperature, float requiredIndoorTemperature, float requiredBoilerTemperature) {
    if(indoorTemperature < requiredIndoorTemperature - 2)
        return requiredBoilerTemperature + 5;
    else if(indoorTemperature < requiredIndoorTemperature - 1)
        return requiredBoilerTemperature + 4;
    else if(indoorTemperature < requiredIndoorTemperature - 0.2)
        return requiredBoilerTemperature + 3;
    else if(indoorTemperature < requiredIndoorTemperature - 0.05)
        return requiredBoilerTemperature + 2;
    else if(indoorTemperature < requiredIndoorTemperature + 0.05)
        return requiredBoilerTemperature;
    else if(indoorTemperature < requiredIndoorTemperature + 0.2)
        return requiredBoilerTemperature - 2;
    else if(indoorTemperature < requiredIndoorTemperature + 1)
        return requiredBoilerTemperature - 3;
    else if(indoorTemperature < requiredIndoorTemperature + 2)
        return requiredBoilerTemperature - 4;
    return requiredBoilerTemperature - 5;
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
        stringstream message_stream;
        message_stream << "Wrong Now. Hour: " << requiredDateTime->tm_hour << "; WeekDay:" << requiredDateTime->tm_wday << endl;
        sd_journal_print(LOG_INFO, message_stream.str().c_str());
        return 20;
    } else {
        return result->Temperature;
    }
}
float Management::GetRequiredBoilerTemperature(int sun, int wind, float outdoorTemperature, float indoorTemperature) {
    float i1 = std::max(minIndoorTemperature, round(indoorTemperature - 0.49f));
    float i2 = std::min(maxIndoorTemperature, round(indoorTemperature + 0.49f));
    if(i1 == i2) {
        if(i1 == minIndoorTemperature) {
            i2 = i1 + 1;
        } else {
            i1 = i2 - 1;
        }
    }
    float o1 = std::max(minOutdoorTemperature, round(outdoorTemperature - 0.49f));
    float o2 = std::min(maxOutdoorTemperature, round(outdoorTemperature + 0.49f));
    if(o1 == o2) {
        if(o1 == minOutdoorTemperature) {
            o2 = o1 + 1;
        } else {
            o1 = o2 - 1;
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
    float result = C * fR1 + D * fR2;
    result -= GetSunAdjust(sun);
    result += GetWindAdjust(wind);
    return result;
}
float Management::GetSunAdjust(int sun) {
    if(sun < 30)
        return 0;
    else if(sun < 50)
        return 1;
    else if(sun < 70)
        return 2;
    else if(sun < 80)
        return 2;
    else if(sun < 90)
        return 3;
    else
        return 3;
}
float Management::GetWindAdjust(int wind) {
    if(wind < 10)
        return 0;
    else if(wind < 20)
        return 2;
    else if(wind < 30)
        return 3;
    else
        return 4;
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
        stringstream message_stream;
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
        stringstream ss;
        ss << "Template read exception." << e.what();
        sd_journal_print(LOG_ERR, ss.str().c_str());
    }
}

Management::~Management() {
    delete settingsTable;
    delete controlTable;
}
