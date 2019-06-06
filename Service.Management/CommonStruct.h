#pragma once

enum CycleResult {
    Starting = 0,
    Normal = 1,
    TimeOut = 2,
    TemperatureLimit = 3
};

enum SensorId {
    Undefined = 0,
    RadioOutdoor = 113,
    RadioIndoor = 150,
    RadioBoiler = 179,
    DirectBoiler = 9000,
    DirectIndoor = 9100,
    GlobalSun = 20000,
    GlobalWind = 20001
};
const SensorId Sensors[] = { RadioOutdoor, RadioIndoor, RadioBoiler, DirectBoiler, DirectIndoor, GlobalSun, GlobalWind };

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