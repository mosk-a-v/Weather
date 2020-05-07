#pragma once
#include "Common.h"
#include "GlobalWeather.h"
#include "Storage.h"
#include "CycleInfo.h"
#include "SensorValues.h"
#include "TemplateUtils.h"
#include "Command.h"
#include "NormalTemperatureStrategy.h"

class Management {
private:
    Storage *storage = nullptr;
    CycleInfo *cycleInfo = nullptr;
    SensorValues *sensorValues = nullptr;
    GlobalWeather *globalWeatherSystem = nullptr;;
    TemperatureStrategyBase *temperatureStrategy = nullptr;
    char *additionalInfo = new char[250];
    std::string statusTemplate;
    SensorId boilerSensorId = DirectBoiler;
    void BeginNewCycle(const time_t &now);
    void StoreGlobalWeather();
public:
    Management(Storage *storage, GlobalWeather *globalWeatherSystem);
    ~Management();

    void ProcessResponce(const DeviceResponce& responce);
    void ProcessCommand(Command *command);
    
};

