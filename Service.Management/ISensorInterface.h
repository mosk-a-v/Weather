#include "StandartLib.h"
#include "Constants.h"
#include "CommonStruct.h"

class ISensorInterface {
protected:
    std::string deviceId;
    SensorId sensorId;
public:
    virtual float Read() = 0;
    SensorId GetSensorId() {
        return sensorId;
    }
    ISensorInterface(std::string deviceId, SensorId sensorId) {
        this->deviceId = deviceId;
        this->sensorId = sensorId;
    }
    virtual ~ISensorInterface() = default;
};