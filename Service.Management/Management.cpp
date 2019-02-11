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
    wiringPiSetupGpio();
    pinMode(PIN, OUTPUT);
    digitalWrite(PIN, !(this->isBoilerOn));
    std::ifstream templateStream(TEMPLATE_FILE_NAME);
    if(!templateStream) {
        cerr << "Error reading " << TEMPLATE_FILE_NAME << endl;
        this->statusTemplate = "";
    } else {
        templateStream >> statusTemplate;
        templateStream.close();
    }
#endif
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
    if(indoorTemperature == DEFAULT_TEMPERATURE || outdoorTemperature == DEFAULT_TEMPERATURE) {
        return;
    }
    float requiredIndoorTemperature = GetRequiredIndoorTemperature();
    float requiredBoilerTemperature = GetRequiredBoilerTemperature(sun, wind, outdoorTemperature, requiredIndoorTemperature);
    float adjustBoilerTemperature = GetAdjustBoilerTemperature(requiredBoilerTemperature, requiredIndoorTemperature);
    delta += CalclateDelta(actualilerTemperature, adjustBoilerTemperature, now);
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
    WriteCurrentStatus(requiredIndoorTemperature, requiredBoilerTemperature, adjustBoilerTemperature);
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
        cerr << "Wrong Now. Hour: " << now->tm_hour << "; WeekDay:" << now->tm_wday << endl;
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
        cerr << "Wrong Temperature. Sun: " << sun << "; Wind:" << wind << "; Outdoor:" << outdoorTemperature << "; Indoor:" << indoorTemperature << endl;
        return 40;
    } else {
        return result->Boiler;
    }
}
float Management::GetAdjustBoilerTemperature(float requiredBoilerTemperature, float requiredIndoorTemperature) {
    if(indoorTemperature < requiredIndoorTemperature - 2)
        return requiredBoilerTemperature + 4;
    else if(indoorTemperature < requiredIndoorTemperature - 1)
        return requiredBoilerTemperature + 3;
    else if(indoorTemperature < requiredIndoorTemperature - 0.2)
        return requiredBoilerTemperature + 2;
    else if(indoorTemperature < requiredIndoorTemperature + 0.2)
        return requiredBoilerTemperature;
    else if(indoorTemperature < requiredIndoorTemperature + 1)
        return requiredBoilerTemperature - 2;
    else if(indoorTemperature < requiredIndoorTemperature + 2)
        return requiredBoilerTemperature - 3;
    return requiredBoilerTemperature - 4;
}
void Management::WriteCurrentStatus(float requiredIndoorTemperature, float requiredBoilerTemperature, float adjustBoilerTemperature) {

    std::ofstream statusStream(OUTPUT_FILE_NAME);
    if(!statusStream) {
        cerr << "Error writing to " << OUTPUT_FILE_NAME << endl;
    } else {
        if(statusTemplate.length() < 10) {
            statusStream << "InA:" << indoorTemperature << "; Out: " << outdoorTemperature << "; BA: " << boilerTemperature <<
                "; BR: " << requiredBoilerTemperature << "; AdjustBoilerTemperature: " << adjustBoilerTemperature <<
                "; Delta: " << delta << "; Status: " << isBoilerOn << endl;
        } else {
            ApplyTemplateAndWrite(statusStream, requiredBoilerTemperature, adjustBoilerTemperature);
        }
        statusStream.close();
    }
}
void Management::ApplyTemplateAndWrite(std::ostream &stream, float requiredBoilerTemperature, float adjustBoilerTemperature) {
    std::string indoorName = "indoor";
    std::string outdoorName = "outdoor";
    std::string boilerName = "boiler";
    std::string requiredBoilerName = "requiredBoiler";
    std::string adjustBoilerName = "adjustBoiler";
    std::string deltaName = "delta";
    std::string stateName = "state";

    std::size_t paramStart = statusTemplate.find("%");
    std::size_t paramEnd = -1;
    while(paramStart != std::string::npos) {
        stream << statusTemplate.substr(paramEnd + 1, paramStart - paramEnd - 1);
        paramEnd = statusTemplate.find("%", paramStart + 1);
        if(paramEnd == std::string::npos)  break;
        string paramName = statusTemplate.substr(paramStart + 1, paramEnd - paramStart - 1);
        if(paramName == indoorName)
            stream << indoorTemperature;
        else if(paramName == outdoorName)
            stream << outdoorTemperature;
        else if(paramName == boilerName)
            stream << boilerTemperature;
        else if(paramName == requiredBoilerName)
            stream << requiredBoilerTemperature;
        else if(paramName == adjustBoilerName)
            stream << adjustBoilerTemperature;
        else if(paramName == deltaName)
            stream << delta;
        else if(paramName == stateName)
            stream << isBoilerOn;
        else
            stream << "Wrong Parameter Name";
        paramStart = statusTemplate.find("%", paramEnd + 1);
    }
    stream << statusTemplate.substr(paramEnd + 1, statusTemplate.length() - paramEnd - 1);
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
