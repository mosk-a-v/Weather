#pragma once
#include "Common.h"
#include "Utils.h"
#include "Management.h"
#include "IInputInterface.h"
#include "MqttConsumer.h"

class Rtl433Input : public IInputInterface {
    const std::string Topic{ "RTL_433/JSON" };
    const std::string ClientId{ "rtl_433_consume" };
    
private:
    std::vector<std::string> models;
    std::map<std::string, SensorInfo> *sensorsTable;
    MqttConsumer consumer = MqttConsumer(Topic, ClientId);
    bool ParseMessage(DeviceResponce& responce, std::string jsonStr);
public:
    Rtl433Input(Management *management, std::map<std::string, SensorInfo> *sensorsTable);
    ~Rtl433Input();
    void Stop();
    void Start();
};