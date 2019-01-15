#include "Management.h"

Management::Management(const std::vector<ControlValue>& controlTable, const std::vector<SettingValue>& settingsTable) {
    this->controlTable = new std::vector<ControlValue>(controlTable);
    this->settingsTable = new std::vector<SettingValue>(settingsTable);
    auto min_max_indoor = std::minmax_element(this->controlTable->begin(), this->controlTable->end(),
                                              [](const ControlValue &c1, const ControlValue &c2) -> bool { return c1.Indoor < c2.Indoor; });
    auto min_max_outdoor = std::minmax_element(this->controlTable->begin(), this->controlTable->end(),
                                               [](const ControlValue &c1, const ControlValue &c2) -> bool { return c1.Outdoor < c2.Outdoor; });
    this->minIndoorTemperature = min_max_indoor.first->Indoor;
    this->maxIndoorTemperature = min_max_indoor.second->Indoor;
    this->minOutdoorTemperature = min_max_outdoor.first->Outdoor;
    this->maxOutdoorTemperature = min_max_outdoor.second->Outdoor;
#ifndef TEST
    wiringPiSetupSys();
    digitalWrite(PIN, !(this->isBoilerOn));
#endif
    this->indoorTemperature = GetRequiredIndoorTemperature();
    this->boilerTemperature = GetRequiredBoilerTemperature(this->sun, this->wind, this->outdoorTemperature, this->indoorTemperature);
    this->lastBoilerResponseTime = GetTime();
}

void Management::ProcessResponce(const DeviceResponce& responce) {
    std::time_t now = GetTime();
    switch(responce.Sensor) {
        case Boiler:
            ManageBoiler(responce.Value, now);
            boilerTemperature = responce.Value;
            lastBoilerResponseTime = now;
            break;
        case Indoor:
            indoorTemperature = responce.Value;
            break;
        case Outdoor:
            outdoorTemperature = responce.Value;
            break;
        default:
            break;
    }
}
void Management::ManageBoiler(float actualilerTemperature, std::time_t now) {
    float requiredIndoorTemperature = GetRequiredIndoorTemperature();
    float preuseIndoorTemperature = requiredIndoorTemperature + 2 * (requiredIndoorTemperature - indoorTemperature);
    float requiredBoilerTemperature = GetRequiredBoilerTemperature(sun, wind, outdoorTemperature, preuseIndoorTemperature);
    delta += CalclateDelta(actualilerTemperature, requiredBoilerTemperature, now);
    if(delta < -MAX_DELTA_DEVIATION) {
        delta = -MAX_DELTA_DEVIATION;
    }
    if(delta > MAX_DELTA_DEVIATION) {
        delta = MAX_DELTA_DEVIATION;
    }
    if(isBoilerOn) {
        if(delta > 0 || actualilerTemperature > (requiredBoilerTemperature + MAX_TEMPERATURE_DEVIATION)) {
            isBoilerOn = false;
            delta = 0;
        }
    } else {
        if(delta < 0 || actualilerTemperature < (requiredBoilerTemperature - MAX_TEMPERATURE_DEVIATION)) {
            isBoilerOn = true;
            delta = 0;
        }
    }
#ifndef TEST
    cout << "InA:" << indoorTemperature << "; InP: " << preuseIndoorTemperature << "; Out: " << outdoorTemperature << "; BA: " << boilerTemperature << "; BR: " << requiredBoilerTemperature << "; Delta:  " << delta << "; Status: " << isBoilerOn << endl;
    digitalWrite(PIN, !isBoilerOn);
#endif
}
float Management::CalclateDelta(float actualilerTemperature, float requiredBoilerTemperature, std::time_t now) {
    float result = 0;
    if(boilerTemperature < actualilerTemperature) {
        if(actualilerTemperature <= requiredBoilerTemperature) {
            result -= (actualilerTemperature - boilerTemperature) * (now - lastBoilerResponseTime) / 2;
        } else if(boilerTemperature < requiredBoilerTemperature && actualilerTemperature > requiredBoilerTemperature) {
            float dx1 = (requiredBoilerTemperature - boilerTemperature) * (now - lastBoilerResponseTime) / (actualilerTemperature - boilerTemperature);
            result -= dx1 * (requiredBoilerTemperature - boilerTemperature) / 2;
            float dx2 = actualilerTemperature - boilerTemperature - dx1;
            result += dx2 * (actualilerTemperature - requiredBoilerTemperature) / 2;
        } else {
            result += (actualilerTemperature - boilerTemperature) * (now - lastBoilerResponseTime) / 2;
        }
    }
    if(boilerTemperature > actualilerTemperature) {
        if(actualilerTemperature >= requiredBoilerTemperature) {
            result += (boilerTemperature - actualilerTemperature) * (now - lastBoilerResponseTime) / 2;
        } else if(actualilerTemperature < requiredBoilerTemperature && boilerTemperature > requiredBoilerTemperature) {
            float dx1 = (boilerTemperature - requiredBoilerTemperature) * (now - lastBoilerResponseTime) / (boilerTemperature - actualilerTemperature);
            result += dx1 * (boilerTemperature - requiredBoilerTemperature) / 2;
            float dx2 = boilerTemperature - actualilerTemperature - dx1;
            result -= dx2 * (requiredBoilerTemperature - actualilerTemperature) / 2;
        } else {
            result -= (boilerTemperature - actualilerTemperature) * (now - lastBoilerResponseTime) / 2;
        }
    }
    return result;
}
float Management::GetRequiredIndoorTemperature() {
    std::tm *now = GetDate();
    std::vector<SettingValue>::iterator result = std::find_if(settingsTable->begin(), settingsTable->end(),
                                                              [now](const SettingValue& s) -> bool {
        return s.Hour == now->tm_hour && s.WeekDay == ((now->tm_wday + 6) % 7 + 1);
    });
    if(result == settingsTable->end()) {
        cout << "Wrong Now. Hour: " << now->tm_hour << "; WeekDay:" << now->tm_wday << endl;
        return 20;
    } else {
        return result->Temperature;
    }
}
float Management::GetRequiredBoilerTemperature(short sun, short wind, float outdoorTemperature, float indoorTemperature) {
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
    return C * fR1 + D * fR2;
}
float Management::GetControlValue(short sun, short wind, float outdoorTemperature, float indoorTemperature) {
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

Management::~Management() {
    delete settingsTable;
    delete controlTable;
}
