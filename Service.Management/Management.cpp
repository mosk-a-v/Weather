﻿#include "Management.h"

Management::Management(Storage *storage, GlobalWeather *globalWeatherSystem) {
    this->storage = storage;
    this->globalWeatherSystem = globalWeatherSystem;
    this->sensorValues = new SensorValues();
    this->cycleInfo = new CycleInfo(Utils::GetTime());
    this->temperatureStrategy = new NormalTemperatureStrategy();
    this->infoPublisher = new MqttPublisher(Topic, ClientId);
    sprintf(additionalInfo, "");
    Utils::SetupGPIO();
    this->statusTemplate = Utils::ReadFile(TEMPLATE_FILE_NAME);
    StoreGlobalWeather();
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
    if(cycleInfo->IsCycleEnd() && temperatureStrategy->GetIndoorTemperature(sensorValues) != DEFAULT_TEMPERATURE) {
        BeginNewCycle(now);
    }
    Utils::SetGPIOValues(BOILER_STATUS_PIN, !cycleInfo->IsBoilerOn());
    TemplateUtils::WriteCurrentStatus(sensorValues, cycleInfo, infoPublisher, statusTemplate, additionalInfo, now);
}
void Management::ProcessCommand(Command *command) {
    std::lock_guard<std::mutex> lock(management_lock);
    time_t now = Utils::GetTime();
    if(temperatureStrategy != nullptr) {
        delete temperatureStrategy;
    }
    temperatureStrategy = command->GetTemperatureStrategy();
    BeginNewCycle(now);
    Utils::SetGPIOValues(BOILER_STATUS_PIN, !cycleInfo->IsBoilerOn());
    TemplateUtils::WriteCurrentStatus(sensorValues, cycleInfo, infoPublisher, statusTemplate, additionalInfo, now);

}
void Management::StoreGlobalWeather() {
    CurrentWeather *weather = globalWeatherSystem->GetWeather();
    sensorValues->AddSpecialSensorValue(GlobalSun, weather->GetSun(), weather->LastUpdateTime);
    sensorValues->AddSpecialSensorValue(GlobalWind, weather->GetWind(), weather->LastUpdateTime);
    sensorValues->AddSpecialSensorValue(GlobalOutdoor, weather->GetTemperature(), weather->LastUpdateTime);
    delete weather;
    weather = nullptr;
}
float Management::ApplayLimitsToBoilerTemperature(float adjustBoilerTemperature, CycleStatictics* lastCycleStat) {
    float const LimitToSecond = 1.0 / (60.0 * 60.0);
    if(lastCycleStat->Result != Normal) {
        return adjustBoilerTemperature;
    }
    float lastCycleLimit = LimitToSecond * (lastCycleStat->CycleLength);
    if(adjustBoilerTemperature > lastCycleStat->BoilerRequired + lastCycleLimit) {
        return lastCycleStat->BoilerRequired + lastCycleLimit;
    }
    else if(adjustBoilerTemperature < lastCycleStat->BoilerRequired - lastCycleLimit) {
        return lastCycleStat->BoilerRequired - lastCycleLimit;
    }
    return adjustBoilerTemperature;
}
void Management::BeginNewCycle(const time_t &now) {
    CycleStatictics *lastCycleStat = cycleInfo->GetStatictics();
    SensorValues *lastSensorValues = sensorValues;
    storage->SaveCycleStatistics(lastCycleStat, lastSensorValues);

    sensorValues = new SensorValues();
    sensorValues->CloneLastValues(lastSensorValues);
    StoreGlobalWeather();
    boilerSensorId = temperatureStrategy->GetBoilerSensorId(lastSensorValues);
    if(boilerSensorId != Undefined) {
        float boilerTemperature = lastSensorValues->GetLastSensorValue(boilerSensorId);
        float minVoltage = lastSensorValues->GetMinSensorValue(ThermocoupleVoltage);
        time_t boilerResponseTime = lastSensorValues->GetLastSensorResponseTime(boilerSensorId);
        float sun = sensorValues->GetLastSensorValue(GlobalSun);
        float wind = sensorValues->GetLastSensorValue(GlobalWind);
        float adjustBoilerTemperature = temperatureStrategy->GetBoilerTemperature(lastSensorValues, sun, wind);
        adjustBoilerTemperature = ApplayLimitsToBoilerTemperature(adjustBoilerTemperature, lastCycleStat);
        bool newCycleWillHeating = boilerTemperature <= adjustBoilerTemperature;
        float indoorTemperature = temperatureStrategy->GetIndoorTemperature(lastSensorValues);
        float outdoorTemperature = temperatureStrategy->GetOutdoorTemperature(lastSensorValues);
        sprintf(additionalInfo, "Boiler: %.2f; Outdoor: %.2f; Indoor: %.2f; %.3f", boilerTemperature, outdoorTemperature, indoorTemperature, minVoltage);
        delete cycleInfo;
        cycleInfo = new CycleInfo(newCycleWillHeating, adjustBoilerTemperature, boilerTemperature, boilerResponseTime, now);
    } else {
        sprintf(additionalInfo, "Sensor error");
        delete cycleInfo;
        cycleInfo = new CycleInfo(now);
    }
    delete lastCycleStat;
    lastCycleStat = nullptr;
    delete lastSensorValues;
    lastSensorValues = nullptr;
    if(statusTemplate.length() < 10) {
        statusTemplate = Utils::ReadFile(TEMPLATE_FILE_NAME);
    }
}
Management::~Management() {
    if(cycleInfo != nullptr) {
        delete cycleInfo;
        cycleInfo = nullptr;
    }
    if(sensorValues != nullptr) {
        delete sensorValues;
        sensorValues = nullptr;
    }
    if(temperatureStrategy != nullptr) {
        delete temperatureStrategy;
        temperatureStrategy = nullptr;
    }
    if (infoPublisher != nullptr) {
        delete infoPublisher;
        infoPublisher = nullptr;
    }
    globalWeatherSystem = nullptr;
    storage = nullptr;
}
