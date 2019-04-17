#pragma once
#include "Management.h"
#include "DS18B20Interface.h"

#define SENSOR_ID "030797794e9d"
#define READ_WAIT 2

class DirectConnectedInput {
private:
    std::atomic<bool> _execute;
    std::thread _thd;
    bool Query(DeviceResponce& responce);
    DS18B20Interface *boilerSensor;
public:
    DirectConnectedInput();
    ~DirectConnectedInput();
    void Stop();
    void Start(int interval, Management *management);
    bool IsRunning() const noexcept;
};

