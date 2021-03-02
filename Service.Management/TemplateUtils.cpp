#include "TemplateUtils.h"

void TemplateUtils::WriteCurrentStatus(SensorValues *sensorValues, CycleInfo *cycle, MqttPublisher *publisher, const std::string statusTemplate, char *additionalInfo, const time_t & now) {
    try {
        PublishStatus(sensorValues, cycle, publisher, additionalInfo, now);
        /*
        std::ofstream statusStream;
        statusStream.open(OUTPUT_FILE_NAME, std::ofstream::out | std::ofstream::trunc);
        if(statusTemplate.length() < 10) {
            CycleStatictics *cycleStat = cycle->GetStatictics();
            statusStream << "InA:" << sensorValues->ToString(RadioBedroom) << "; Out: " << sensorValues->ToString(RadioOutdoor) << "; BA: " << sensorValues->ToString(RadioBoiler) <<
                "; BR: " << cycleStat->BoilerRequired << "; Delta: " << cycleStat->Delta << "; Status: " << cycleStat->IsHeating << std::endl;
            delete cycleStat;
        } else {
            statusStream << FillTemplate(sensorValues, cycle, statusTemplate, additionalInfo, now);
        }
        statusStream.close();
        */
    } catch(std::exception e) {
        Utils::WriteLogInfo(LOG_WARNING, "Status write exception.", "");
    }
}

void TemplateUtils::PublishStatus(SensorValues* sensorValues, CycleInfo* cycle, MqttPublisher* publisher, char* additionalInfo, const time_t& now) {
    CycleStatictics* cycleStat = cycle->GetStatictics();
    nlohmann::json boilerStatistics;
    boilerStatistics["Delta"] = cycleStat->Delta;
    boilerStatistics["CycleStart"] = cycleStat->CycleStart;
    boilerStatistics["IsBoilerOn"] = cycleStat->IsBoilerOn;
    boilerStatistics["BoilerRequired"] = cycleStat->BoilerRequired;
    boilerStatistics["IsHeating"] = cycleStat->IsHeating;
    boilerStatistics["AdditionalInfo"] = additionalInfo;
    boilerStatistics["Sensors"] = sensorValues->ToJson();
    publisher->Publish(boilerStatistics.dump());
    delete cycleStat;
}

std::string TemplateUtils::FillTemplate(SensorValues *sensorValues, CycleInfo *cycle, const std::string statusTemplate, char *additionalInfo, const time_t & now) {
    std::stringstream ss;
    ss << std::fixed;
    ss.precision(1);
    ss.imbue(std::locale(std::locale("ru_RU.utf8"), new IntegerNumPunct));
    CycleStatictics* cycleStat = cycle->GetStatictics();
    size_t paramStart = statusTemplate.find("%");
    size_t paramEnd = -1;
    while(paramStart != std::string::npos) {
        ss << statusTemplate.substr(paramEnd + 1, paramStart - paramEnd - 1);
        paramEnd = statusTemplate.find("%", paramStart + 1);
        if(paramEnd == std::string::npos)  break;
        std::string paramName = statusTemplate.substr(paramStart + 1, paramEnd - paramStart - 1);
        try {
            int sensorId = (SensorId)std::stoi(paramName);
            std::string value = sensorValues->ToString((SensorId)sensorId);
            if(value.length() > 1) {
                ss << value;
            } else {
                ss << "Wrong Parameter Name";
            }
        } catch(...) {
            if(paramName == "time") {
                tm tm = *localtime(&now);
                ss << std::put_time(&tm, "%c");
            } else if(paramName == "delta") {
                ss << cycleStat->Delta << " (" << now - cycleStat->CycleStart << ")";
            } else if(paramName == "state") {
                ss << cycleStat->IsBoilerOn;
            } else if(paramName == "periodType") {
                ss << (cycleStat->BoilerRequired == DEFAULT_TEMPERATURE ? "Запуск" : (cycleStat->IsHeating ? "Нагрев" : "Охлаждение"));
            } else if(paramName == "requiredBoiler") {
                if(cycleStat->BoilerRequired == DEFAULT_TEMPERATURE) {
                    ss << "---";
                } else {
                    ss << cycleStat->BoilerRequired;
                }
            } else if(paramName == "additionalInfo") {
                ss << additionalInfo;
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

