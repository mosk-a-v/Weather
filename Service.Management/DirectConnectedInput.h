#pragma once
#include "Common.h"
#include "Management.h"

class DirectConnectedInput {
private:
    std::atomic<bool> _execute;
    std::thread _thd;
    bool Query(DeviceResponce& responce);
    ISensorInterface *sensor;
    Management *management;
public:
    DirectConnectedInput(Management *management, ISensorInterface *sensor);
    ~DirectConnectedInput();
    void Stop();
    void Start();
    bool IsRunning() const noexcept;
};

