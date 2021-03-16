#pragma once
#include "Common.h"
#include "Utils.h"
#include "SensorValues.h"

class Storage {
private:
    sql::Driver *driver;
    sql::Connection *connection;
    void LogException(std::exception &e);
    void Connect();
    void SaveCycleStatisticsInternal(CycleStatictics *cycleStat, SensorValues *sensorValues);
public:
    Storage();
    ~Storage();
    void SaveCycleStatistics(CycleStatictics *cycleStat, SensorValues *sensorValues);
    static std::vector<ControlValue> *ReadControlTable();
    static std::vector<SettingValue> *ReadSettingsTable();
    static std::map<std::string, SensorInfo>* ReadSensorsTable();
};

