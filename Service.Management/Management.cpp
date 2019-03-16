#include "Management.h"

Management::Management(const std::vector<ControlValue>& controlTable, const std::vector<SettingValue>& settingsTable, GlobalWeather *globalWeatherSystem) {
    this->controlTable = new std::vector<ControlValue>(controlTable);
    this->settingsTable = new std::vector<SettingValue>(settingsTable);
    this->globalWeatherSystem = globalWeatherSystem;
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
    cycleInfo = new CycleInfo(false, DEFAULT_TEMPERATURE, GetTime(), statusTemplate, "Starting");
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
    float requiredIndoorTemperature = GetRequiredIndoorTemperature();
    float requiredBoilerTemperature = GetRequiredBoilerTemperature(weather.GetSun(), weather.GetWind(), cycleInfo->GetAverageOutdoorTemperature(), requiredIndoorTemperature);
    float adjustBoilerTemperature = GetAdjustBoilerTemperature(cycleInfo->GetAverageIndoorTemperature(), requiredIndoorTemperature, requiredBoilerTemperature);
    bool newCycleWillHeating = cycleInfo->GetBoilerTemperture() <= adjustBoilerTemperature;
    std::stringstream additionalInfoStream;
    additionalInfoStream << "Ветер: " << weather.GetWind() << "; Солнце: " << weather.GetSun() << ". Прошлый цикл (" << cycleInfo->GetCycleLength() << "c, " << (cycleInfo->IsCycleHeating() ? "нагрев" : "охлаждение") << "). ";
    delete cycleInfo;
    cycleInfo = new CycleInfo(newCycleWillHeating, adjustBoilerTemperature, now, statusTemplate, additionalInfoStream.str());
}
float Management::GetAdjustBoilerTemperature(float indoorTemperature, float requiredIndoorTemperature, float requiredBoilerTemperature) {
    if(indoorTemperature < requiredIndoorTemperature - 2)
        return requiredBoilerTemperature + 7;
    else if(indoorTemperature < requiredIndoorTemperature - 1)
        return requiredBoilerTemperature + 6;
    else if(indoorTemperature < requiredIndoorTemperature - 0.2)
        return requiredBoilerTemperature + 5;
    else if(indoorTemperature < requiredIndoorTemperature - 0.05)
        return requiredBoilerTemperature + 4;
    else if(indoorTemperature < requiredIndoorTemperature + 0.05)
        return requiredBoilerTemperature;
    else if(indoorTemperature < requiredIndoorTemperature + 0.2)
        return requiredBoilerTemperature - 4;
    else if(indoorTemperature < requiredIndoorTemperature + 1)
        return requiredBoilerTemperature - 5;
    else if(indoorTemperature < requiredIndoorTemperature + 2)
        return requiredBoilerTemperature - 6;
    return requiredBoilerTemperature - 7;
}
float Management::GetRequiredIndoorTemperature() {
    std::tm *now = GetDate();
    std::vector<SettingValue>::iterator result = std::find_if(settingsTable->begin(), settingsTable->end(),
                                                              [now](const SettingValue& s) -> bool {
        return s.Hour == now->tm_hour && s.WeekDay == ((now->tm_wday + 6) % 7 + 1);
    });
    if(result == settingsTable->end()) {
        cerr << "Wrong Now. Hour: " << now->tm_hour << "; WeekDay:" << now->tm_wday << endl;
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
    float f11 = GetControlValue(sun, wind, o1, i1);
    float f12 = GetControlValue(sun, wind, o1, i2);
    float f21 = GetControlValue(sun, wind, o2, i1);
    float f22 = GetControlValue(sun, wind, o2, i2);
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
    if(sun < 70)
        return 0;
    else if(sun < 80)
        return 1;
    else if(sun < 90)
        return 2;
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
        cout << "Wrong Temperature. Sun: " << sun << "; Wind:" << wind << "; Outdoor:" << outdoorTemperature << "; Indoor:" << indoorTemperature << endl;
        return 40;
    } else {
        return result->Boiler;
    }
}
std::time_t Management::GetTime() {
    return std::time(0);
}
std::tm* Management::GetDate() {
    std::time_t t = GetTime();
    return std::localtime(&t);
}
void Management::SetupGPIO() {
    wiringPiSetupGpio();
    pinMode(PIN, OUTPUT);
    SetGPIOValues();
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
    } catch(exception e) {
        statusTemplate = "";
        cout << "Template read exception." << endl;
    }
}

Management::~Management() {
    delete settingsTable;
    delete controlTable;
}
