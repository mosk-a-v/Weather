#pragma once
#include "Common.h"
#include "Utils.h"
#include "Management.h"
#include "IInputInterface.h"

class DirectConnectedInput : public IInputInterface {
private:
    ISensorInterface *sensor;
    bool Query(DeviceResponce& responce);
    int queryInterval = QUERY_INTERVAL;
public:
    DirectConnectedInput(Management *management, ISensorInterface *sensor);
    DirectConnectedInput(Management* management, ISensorInterface* sensor, int queryInterval);
    ~DirectConnectedInput();
    void Stop();
    void Start();
};

