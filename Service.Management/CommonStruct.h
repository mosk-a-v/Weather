﻿#pragma once

enum CycleResult {
    Starting = 0,
    Normal = 1,
    TimeOut = 2,
    TemperatureLimit = 3,
    SensorError = 4
};

enum SensorId {
    Undefined = 0,
    RadioOutdoor = 1,
    RadioBedroom = 2,
    RadioBoiler = 3,
    RadioLounge = 4,
    RadioMansard = 5,
    RadioStudy = 6,
    DirectBoiler = 7,
    DirectIndoor = 8,
    DirectOtdoor = 9,
    RadioKitchen = 10,
    RadioLoungeHumidity = 24,
    RadioMansardHumidity = 25,
    RadioStudyHumidity = 26,
    RadioKitchenHumidity = 30,
    GlobalSun = 100,
    GlobalWind = 101,
    GlobalOutdoor = 102,
    ThermocoupleVoltage = 103,
    OtherKithen = 104,
    OtherBoiler = 105
};

struct ControlValue {
public:
    int Sun;
    int Wind;
    float Indoor;
    float Outdoor;
    float Boiler;
};

struct SettingValue {
public:
    short WeekDay;
    short Hour;
    float Temperature;
};

struct CurrentWeather {
public:
    std::time_t SunRise;
    std::time_t SunSet;
    std::time_t LastUpdateTime;
    std::time_t ResponceTime;
    float TemperatureValue;
    float TemperatureMin;
    float TemperatureMax;
    float WindSpeed = 0;
    int Clouds;
    bool IsServiceError;
    int GetSun();
    int GetWind();
    float GetTemperature();
    float GetAverageMonthTemperature(int month);
    float GetDailyShift(int hour);
};

struct CycleStatictics {
public:
    time_t CycleStart;
    float BoilerRequired;
    bool IsHeating;
    bool IsBoilerOn;
    int CycleLength;
    CycleResult Result;
    float Delta;
    float MaxDelta;
};

struct DeviceResponce {
public:
    std::string Time;
    SensorId Sensor;
    float Value;
    int Humidity;
    bool Battery;

    DeviceResponce() {
        Time = "";
        Sensor = Undefined;
        Value = 0;
    }
    DeviceResponce(SensorId sensor, float value) {
        Time = "";
        Sensor = sensor;
        Value = value;
    }
    bool operator==(const DeviceResponce& right) {
        return Sensor == right.Sensor && Value == right.Value;
    }
};

struct SensorInfo {
public:
    SensorId Id;
    bool UseForCalc;
    bool IsIndoor;
    bool IsOutdoor;
    std::string InterfaceName;
    float CorrectionCoefficient;
    float Shift;

    SensorInfo(int id, bool isIndoor, bool isOutdoor, bool useForCalc, std::string interfaceName, float correctionCoefficient, float shift) {
        Id = (SensorId)id;
        IsIndoor = isIndoor;
        IsOutdoor = isOutdoor;
        UseForCalc = useForCalc;
        InterfaceName = interfaceName;
        CorrectionCoefficient = correctionCoefficient;
        Shift = shift;
    }
};