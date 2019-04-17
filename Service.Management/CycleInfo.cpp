#include "CycleInfo.h"

void CycleInfo::CalculateDelta(float boilerTemperature, const time_t& now) {
    deltaForLastPeriod = CalclateDeltaForLastPeriod(boilerTemperature, now);
    delta += deltaForLastPeriod;
    if(delta < -MAX_DELTA_DEVIATION) {
        delta = -MAX_DELTA_DEVIATION;
    }
    if(delta > MAX_DELTA_DEVIATION) {
        delta = MAX_DELTA_DEVIATION;
    }
}
void CycleInfo::DetectLatency() {
    if(isLatencyPeriod) {
        return;
    }
    if(isHeating) {
        if(delta > -latency && deltaForLastPeriod > 0) {
            isLatencyPeriod = true;
        }
    } else {
        if(delta < latency && deltaForLastPeriod < 0) {
            isLatencyPeriod = true;
        }
    }
}
float CycleInfo::CalclateDeltaForLastPeriod(float boilerTemperature, const time_t& now) {
    float result = 0;
    if(lastBoilerResponseTime == DEFAULT_TIME || lastBoilerTemperature == DEFAULT_TEMPERATURE || requiredBoilerTemperature == DEFAULT_TEMPERATURE) {
        return result;
    }
    if(lastBoilerTemperature < boilerTemperature) {
        if(boilerTemperature <= requiredBoilerTemperature) {
            result -= (boilerTemperature - lastBoilerTemperature) * (now - lastBoilerResponseTime) / 2;
        } else if(lastBoilerTemperature < requiredBoilerTemperature && boilerTemperature > requiredBoilerTemperature) {
            float dx1 = (requiredBoilerTemperature - lastBoilerTemperature) * (now - lastBoilerResponseTime) / (boilerTemperature - lastBoilerTemperature);
            result -= dx1 * (requiredBoilerTemperature - lastBoilerTemperature) / 2;
            float dx2 = boilerTemperature - lastBoilerTemperature - dx1;
            result += dx2 * (boilerTemperature - requiredBoilerTemperature) / 2;
        } else {
            result += (boilerTemperature - lastBoilerTemperature) * (now - lastBoilerResponseTime) / 2;
        }
    }
    if(lastBoilerTemperature > boilerTemperature) {
        if(boilerTemperature >= requiredBoilerTemperature) {
            result += (lastBoilerTemperature - boilerTemperature) * (now - lastBoilerResponseTime) / 2;
        } else if(boilerTemperature < requiredBoilerTemperature && lastBoilerTemperature > requiredBoilerTemperature) {
            float dx1 = (lastBoilerTemperature - requiredBoilerTemperature) * (now - lastBoilerResponseTime) / (lastBoilerTemperature - boilerTemperature);
            result += dx1 * (lastBoilerTemperature - requiredBoilerTemperature) / 2;
            float dx2 = lastBoilerTemperature - boilerTemperature - dx1;
            result -= dx2 * (requiredBoilerTemperature - boilerTemperature) / 2;
        } else {
            result -= (lastBoilerTemperature - boilerTemperature) * (now - lastBoilerResponseTime) / 2;
        }
    }
    return result;
}
void CycleInfo::WriteCurrentStatus(const time_t& now) {
    try {
        std::ofstream statusStream;
        statusStream.open(OUTPUT_FILE_NAME, std::ofstream::out | std::ofstream::trunc);
        if(statusTemplate.length() < 10) {
            statusStream << "InA:" << lastIndoorTemperature << "; Out: " << lastOutdoorTemperature << "; BA: " << lastBoilerTemperature <<
                "; BR: " << requiredBoilerTemperature << "; Delta: " << delta << "; Status: " << isHeating << endl;
        } else {
            ApplyTemplateAndWrite(statusStream, now);
        }
        statusStream.close();
    } catch(exception e) {
        sd_journal_print(LOG_ERR, "Status write exception.");
    }
}
void CycleInfo::ApplyTemplateAndWrite(std::ostream &stream, const time_t& now) {
    string indoorName = "indoor";
    string outdoorName = "outdoor";
    string boilerName = "boiler";
    string indoorAvgName = "indoorAvg";
    string outdoorAvgName = "outdoorAvg";
    string requiredBoilerName = "requiredBoiler";
    string deltaName = "delta";
    string stateName = "state";
    string timeName = "time";
    string periodTypeName = "periodType";
    string additionalTemplateParameterName = "additionalTemplateParameter";

    stream << fixed;
    stream.precision(1);

    size_t paramStart = statusTemplate.find("%");
    size_t paramEnd = -1;
    while(paramStart != std::string::npos) {
        stream << statusTemplate.substr(paramEnd + 1, paramStart - paramEnd - 1);
        paramEnd = statusTemplate.find("%", paramStart + 1);
        if(paramEnd == std::string::npos)  break;
        string paramName = statusTemplate.substr(paramStart + 1, paramEnd - paramStart - 1);
        if(paramName == indoorName) {
            if(lastIndoorTemperature == DEFAULT_TEMPERATURE) {
                stream << "---";
            } else {
                stream << lastIndoorTemperature << " (" << FormatTime(lastIndoorResponceTime) << ")";
            }
        } else if(paramName == outdoorName) {
            if(lastOutdoorTemperature == DEFAULT_TEMPERATURE) {
                stream << "---";
            } else {
                stream << lastOutdoorTemperature << " (" << FormatTime(lastOutdoorResponceTime) << ")";
            }
        } else if(paramName == boilerName) {
            if(lastBoilerTemperature == DEFAULT_TEMPERATURE) {
                stream << "---";
            } else {
                stream << lastBoilerTemperature << " (" << FormatTime(lastBoilerResponseTime) << ")"
                    << " [" << lastDirectBoilerTemperature << " (" << FormatTime(lastDirectBoilerResponseTime) << ")]";
            }
        } else if(paramName == requiredBoilerName) {
            if(requiredBoilerTemperature == DEFAULT_TEMPERATURE) {
                stream << "---";
            } else {
                stream << requiredBoilerTemperature;
            }
        } else if(paramName == deltaName) {
            stream << delta << " (" << now - cycleStartTime << ")";
        } else if(paramName == stateName) {
            stream << IsBoilerOn();
        } else if(paramName == timeName) {
            tm tm = *localtime(&lastDeviceResponceTime);
            stream.imbue(locale("ru_RU.utf8"));
            stream << put_time(&tm, "%c");
        } else if(paramName == indoorAvgName) {
            stream << GetAverageIndoorTemperature();
        } else if(paramName == outdoorAvgName) {
            stream << GetAverageOutdoorTemperature();
        } else if(paramName == periodTypeName) {
            stream << (IsStartingMode() ? "Запуск" : (isHeating ? "Нагрев" : "Охлаждение"));
        } else if(paramName == additionalTemplateParameterName) {
            stream << additionalTemplateParameterValue;
        } else {
            stream << "Wrong Parameter Name";
        }
        paramStart = statusTemplate.find("%", paramEnd + 1);
    }
    stream << statusTemplate.substr(paramEnd + 1, statusTemplate.length() - paramEnd - 1);
}
void CycleInfo::EndCycle(CycleResult result, const time_t & now) {
    if(this->isCycleEnd) {
        return;
    }
    this->cycleEndTime = now;
    this->isCycleEnd = true;
    this->result = result;
}
void CycleInfo::AddBoilerTemperatue(float value, const time_t& now) {
    if(value == DEFAULT_TEMPERATURE) {
        return;
    }
    DetectComplitingStartMode(now);
    ProcessBoilerTemperature(value, now);
    lastBoilerTemperature = value;
    if(averageBoilerTemperature == DEFAULT_TEMPERATURE) {
        averageBoilerTemperature = (now - cycleStartTime) * value;
    } else {
        averageBoilerTemperature += (now - lastBoilerResponseTime) * value;
    }
    lastBoilerResponseTime = now;
    lastDeviceResponceTime = now;
    WriteCurrentStatus(now);
}
void CycleInfo::ProcessBoilerTemperature(float value, const time_t& now) {
    if(requiredBoilerTemperature == DEFAULT_TEMPERATURE || isCycleEnd) {
        return;
    }
    CalculateDelta(value, now);
    if(now - cycleStartTime < MIN_CYCLE_TIME) {
        return;
    }
    if(now - cycleStartTime > MAX_CYCLE_TIME) {
        EndCycle(TimeOut, now);
        return;
    }
    if(value > (requiredBoilerTemperature + MAX_TEMPERATURE_DEVIATION) && IsBoilerOn()) {
        EndCycle(TemperatureLimit, now);
    } else if(value < (requiredBoilerTemperature - MAX_TEMPERATURE_DEVIATION) && !IsBoilerOn()) {
        EndCycle(TemperatureLimit, now);
    } else {
        if(isHeating && delta >= 0) {
            EndCycle(Normal, now);
        } else if(!isHeating && delta <= 0) {
            EndCycle(Normal, now);
        } else {
            DetectLatency();
        }
    }
}
bool CycleInfo::IsStartingMode() {
    return requiredBoilerTemperature == DEFAULT_TEMPERATURE;
}
void CycleInfo::DetectComplitingStartMode(const time_t & now) {
    if(IsStartingMode() && !isCycleEnd) {
        if(lastBoilerTemperature != DEFAULT_TEMPERATURE &&
           lastOutdoorTemperature != DEFAULT_TEMPERATURE &&
           lastIndoorTemperature != DEFAULT_TEMPERATURE) {
            EndCycle(Starting, now);
        }
    }
}
string CycleInfo::FormatTime(const time_t& time) {
    stringstream buffer;
    tm tm = *localtime(&time);
    buffer.imbue(locale("ru_RU.utf8"));
    buffer << put_time(&tm, "%T");
    return buffer.str();
}
void CycleInfo::AddIndoorTemperature(float value, const time_t& now) {
    if(value == DEFAULT_TEMPERATURE) {
        return;
    }
    DetectComplitingStartMode(now);
    lastIndoorTemperature = value;
    if(averageIndoorTemperature == DEFAULT_TEMPERATURE) {
        averageIndoorTemperature = (now - cycleStartTime) * value;
    } else {
        averageIndoorTemperature += (now - lastIndoorResponceTime) * value;
    }
    lastDeviceResponceTime = now;
    lastIndoorResponceTime = now;
    WriteCurrentStatus(now);
}
void CycleInfo::AddOutdoorTemperature(float value, const time_t& now) {
    if(value == DEFAULT_TEMPERATURE) {
        return;
    }
    DetectComplitingStartMode(now);
    lastOutdoorTemperature = value;
    if(averageOutdoorTemperature == DEFAULT_TEMPERATURE) {
        averageOutdoorTemperature = (now - cycleStartTime) * value;
    } else {
        averageOutdoorTemperature += (now - lastOutdoorResponceTime) * value;
    }
    lastDeviceResponceTime = now;
    lastOutdoorResponceTime = now;
    WriteCurrentStatus(now);
}
void CycleInfo::AddDirectBoilerTemperature(float value, const time_t & now) {
    if(value == DEFAULT_TEMPERATURE) {
        return;
    }
    //DetectComplitingStartMode(now);
    lastDirectBoilerTemperature = value;
    if(averageDirectBoilerTemperature == DEFAULT_TEMPERATURE) {
        averageDirectBoilerTemperature = (now - cycleStartTime) * value;
    } else {
        averageDirectBoilerTemperature += (now - lastDirectBoilerResponseTime) * value;
    }
    lastDeviceResponceTime = now;
    lastDirectBoilerResponseTime = now;
    WriteCurrentStatus(now);
}
float CycleInfo::GetAverageIndoorTemperature() {
    return (lastIndoorResponceTime == cycleStartTime) ?
        lastIndoorTemperature : averageIndoorTemperature / (lastIndoorResponceTime - cycleStartTime);
}
float CycleInfo::GetAverageOutdoorTemperature() {
    return (lastOutdoorResponceTime == cycleStartTime) ?
        lastOutdoorTemperature : averageOutdoorTemperature / (lastOutdoorResponceTime - cycleStartTime);
}
float CycleInfo::GetAverageBoilerTemperature() {
    return (lastBoilerResponseTime == cycleStartTime) ?
        lastBoilerTemperature : averageBoilerTemperature / (lastBoilerResponseTime - cycleStartTime);
}
float CycleInfo::GetAverageDirectBoilerTemperature() {
    return (lastDirectBoilerResponseTime == cycleStartTime) ?
        lastDirectBoilerTemperature : averageDirectBoilerTemperature / (lastDirectBoilerResponseTime - cycleStartTime);
}
bool CycleInfo::IsBoilerOn() {
    if(IsStartingMode()) {
        return false;
    }
    return (isLatencyPeriod || isCycleEnd) ? !isHeating : isHeating;
}
bool CycleInfo::IsCycleEnd() {
    return isCycleEnd;
}
CycleStatictics* CycleInfo::GetStatictics() {
    CycleStatictics *result = new CycleStatictics();
    result->AvgIndoor = GetAverageIndoorTemperature();
    result->AvgOutdoor = GetAverageOutdoorTemperature();
    result->AvgBoiler = GetAverageBoilerTemperature();
    result->AvgDirectBoiler = GetAverageDirectBoilerTemperature();
    result->BoilerRequired = requiredBoilerTemperature;
    result->CycleLength = cycleEndTime - cycleStartTime;
    result->CycleStart = cycleStartTime;
    result->IsHeating = isHeating;
    result->LastIndoor = lastIndoorTemperature;
    result->LastOutdoor = lastOutdoorTemperature;
    result->LastBoiler = lastBoilerTemperature;
    result->Sun = sun;
    result->Wind = wind;
    result->Result = this->result;
    return result;
}
CycleInfo::CycleInfo(bool isHeating, float requiredBoilerTemperature, CurrentWeather weather, const time_t& now, string statusTemplate, string additionalTemplateParameterValue) {
    this->cycleStartTime = now;
    this->statusTemplate = statusTemplate;
    this->requiredBoilerTemperature = requiredBoilerTemperature;
    this->isHeating = isHeating;
    this->additionalTemplateParameterValue = additionalTemplateParameterValue;
    this->sun = weather.GetSun();
    this->wind = weather.GetWind();
    this->globalTemperature = weather.GetTemperature();
}
