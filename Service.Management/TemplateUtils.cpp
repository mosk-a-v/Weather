#include "TemplateUtils.h"

std::string TemplateUtils::GetSensorValue(SensorValues *sensorValues, SensorId sensorId) {
    float lastValue = sensorValues->GetLastSensorValue(sensorId);
    if(lastValue == DEFAULT_TEMPERATURE) {
        return "---";
    } else {
        std::stringstream ss;
        ss << lastValue << " (" << Utils::FormatTime(sensorValues->GetLastSensorResponseTime(sensorId)) << ")"
            << " [" << sensorValues->GetAveragSensorValue(sensorId) << ")]";
        return ss.str();
    }
}

std::string TemplateUtils::GetAllSensorValues(SensorValues * sensorValues) {
    std::stringstream ss;
    for(const auto& s : Sensors) {
        SensorId sensorId = (SensorId)s;
        ss << sensorId << ": " << GetSensorValue(sensorValues, sensorId) << ". ";
    }
    return ss.str();
}

void TemplateUtils::WriteCurrentStatus(SensorValues *sensorValues, CycleInfo *cycle, const std::string statusTemplate, const time_t & now) {
    try {
        std::ofstream statusStream;
        statusStream.open(OUTPUT_FILE_NAME, std::ofstream::out | std::ofstream::trunc);
        if(statusTemplate.length() < 10) {
            CycleStatictics *cycleStat = cycle->GetStatictics();
            statusStream << "InA:" << GetSensorValue(sensorValues, RadioIndoor) << "; Out: " << GetSensorValue(sensorValues, RadioOutdoor) << "; BA: " << GetSensorValue(sensorValues, RadioBoiler) <<
                "; BR: " << cycleStat->BoilerRequired << "; Delta: " << cycleStat->Delta << "; Status: " << cycleStat->IsHeating << std::endl;
            delete cycleStat;
        } else {
            statusStream << FillTemplate(sensorValues, cycle, statusTemplate, now);
        }
        statusStream.close();
    } catch(std::exception e) {
        sd_journal_print(LOG_ERR, "Status write exception.");
    }
}

std::string TemplateUtils::FillTemplate(SensorValues *sensorValues, CycleInfo *cycle, const std::string statusTemplate, const time_t & now) {
    std::stringstream ss;
    ss << std::fixed;
    ss.precision(1);

    CycleStatictics *cycleStat = cycle->GetStatictics();
    size_t paramStart = statusTemplate.find("%");
    size_t paramEnd = -1;
    while(paramStart != std::string::npos) {
        ss << statusTemplate.substr(paramEnd + 1, paramStart - paramEnd - 1);
        paramEnd = statusTemplate.find("%", paramStart + 1);
        if(paramEnd == std::string::npos)  break;
        std::string paramName = statusTemplate.substr(paramStart + 1, paramEnd - paramStart - 1);
        try {
            int sensorId = (SensorId)std::stoi(paramName);
            std::string value = GetSensorValue(sensorValues, (SensorId)sensorId);
            if(value.length() > 1) {
                ss << value;
            } else {
                ss << "Wrong Parameter Name";
            }
        } catch(...) {
            if(paramName == "time") {
                tm tm = *localtime(&now);
                ss.imbue(std::locale("ru_RU.utf8"));
                ss << std::put_time(&tm, "%c");
            } else if(paramName == "delta") {
                ss << cycleStat->Delta << " (" << now - cycleStat->CycleStart << ")";
            } else if(paramName == "state") {
                ss << cycleStat->IsBoilerOn;
            } else if(paramName == "periodType") {
                ss << (cycleStat->BoilerRequired == DEFAULT_TEMPERATURE ? "Запуск" : (cycleStat->IsHeating ? "Нагрев" : "Охлаждение"));
            } else {
                ss << "Wrong Parameter Name";
            }
        }
        paramStart = statusTemplate.find("%", paramEnd + 1);
    }
    ss << statusTemplate.substr(paramEnd + 1, statusTemplate.length() - paramEnd - 1);
    delete cycleStat;
    return ss.str();
}

