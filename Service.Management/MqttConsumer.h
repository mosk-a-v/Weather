#pragma once
#include "mqtt/async_client.h"
#include "Common.h"

class MqttConsumer {
private:
    std::string topic;
    std::string clientId;
    mqtt::async_client *client = nullptr;
    mqtt::connect_options connOpts;
public:
    MqttConsumer(std::string topic, std::string clientId);
    ~MqttConsumer();

    void Connect();
    void Disconnect();
    std::string ReadMessage();
};