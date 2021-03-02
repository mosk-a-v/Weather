#pragma once
#include <stdlib.h>
#include <iostream>
#include "Common.h"
#include "Utils.h"


class DS18B20Interface : public ISensorInterface {
    FILE *file = nullptr;
    float correctionCoefficient = 1;
    float shift = 0;
    void FileOpen();
    void CloseFile();
    void ResetSensor();
public:
    float Read();
    DS18B20Interface(std::string deviceId, SensorId sensorId, float correctionCoefficient, float shift);
    ~DS18B20Interface();
};