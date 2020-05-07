#pragma once
#include "Common.h"
#include "Management.h"
#include "IInputInterface.h"

class DirectConnectedInput : public IInputInterface {
private:
    ISensorInterface *sensor;
    bool Query(DeviceResponce& responce);
public:
    DirectConnectedInput(Management *management, ISensorInterface *sensor);
    ~DirectConnectedInput();
    void Stop();
    void Start();
};

