#pragma once
#include "Common.h"
#include "Management.h"
#include "mqtt/async_client.h"

const std::string SERVER_ADDRESS{ "tcp://localhost:1883" };
const std::string CLIENT_ID{ "async_consume" };
const std::string TOPIC{ "RTL_433/JSON" };
const int  QOS = 1;

class Rtl433Input {
private:
    std::atomic<bool> _execute;
    std::thread _thd;
    mqtt::async_client *client;
    mqtt::connect_options connOpts;
    bool ParseMessage(DeviceResponce& responce, std::string jsonStr);
    Management *management;
    std::map<std::string, SensorInfo> *sensorsTable;
    void Connect();
    std::string ReadMessage();
public:
    Rtl433Input(Management *management, std::map<std::string, SensorInfo> *sensorsTable);
    ~Rtl433Input();
    void Stop();
    void Disconnect();
    void Start();
    bool IsRunning() const noexcept;
};


