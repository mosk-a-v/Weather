#pragma once
#include "Common.h"
#include "SensorValues.h"

class Storage {
private:
    sql::Driver *driver;
    sql::Connection *connection;
    void LogException(std::exception &e);
    void Connect();
    void SaveResponceInternal(const DeviceResponce& responce);
    void SaveCycleStatisticsInternal(CycleStatictics *cycleStat, SensorValues *sensorValues);
public:
    Storage();
    ~Storage();
    void SaveResponce(const DeviceResponce& responce);
    void SaveCycleStatistics(CycleStatictics *cycleStat, SensorValues *sensorValues);
    std::vector<ControlValue> *ReadControlTable();
    std::vector<SettingValue> *ReadSettingsTable();
};

