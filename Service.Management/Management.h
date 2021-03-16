#pragma once
#include "Common.h"
#include "Utils.h"
#include "GlobalWeather.h"
#include "Storage.h"
#include "CycleInfo.h"
#include "SensorValues.h"
#include "TemplateUtils.h"
#include "Command.h"
#include "NormalTemperatureStrategy.h"
#include "MqttPublisher.h"

class Management {
    const std::string Topic{ "BOILER_STATISTICS/JSON" };
    const std::string ClientId{ "boiler_stat_publisher" };
private:
    Storage *storage = nullptr;
    CycleInfo *cycleInfo = nullptr;
    SensorValues *sensorValues = nullptr;
    GlobalWeather *globalWeatherSystem = nullptr;;
    TemperatureStrategyBase *temperatureStrategy = nullptr;
    MqttPublisher *infoPublisher = nullptr;
    char *additionalInfo = new char[250];
    std::string statusTemplate;
    SensorId boilerSensorId = DirectBoiler;
    void BeginNewCycle(const time_t &now);
    void StoreGlobalWeather();
    float ApplayLimitsToBoilerTemperature(float adjustBoilerTemperature, CycleStatictics* lastCycleStat);
public:
    Management(Storage *storage, GlobalWeather *globalWeatherSystem);
    ~Management();

    void ProcessResponce(const DeviceResponce& responce);
    void ProcessCommand(Command *command);
    
};

