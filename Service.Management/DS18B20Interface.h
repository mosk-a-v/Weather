#pragma once
#include <stdlib.h>
#include <iostream>
#pragma once
#include "Common.h"

class DS18B20Interface : public ISensorInterface {
    FILE *file = nullptr;
    void FileOpen();
public:
    float Read();
    DS18B20Interface(std::string deviceId, SensorId sensorId);
    ~DS18B20Interface();
};