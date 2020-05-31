#pragma once
#include "mqtt/async_client.h"
#include "Common.h"

class MqttPublisher {
private:
    std::string topic;
    std::string clientId;
    mqtt::async_client* client = nullptr;
    mqtt::connect_options connOpts;
public:
    MqttPublisher(std::string topic, std::string clientId);
    ~MqttPublisher();

    void Connect();
    void Disconnect();
    void Publish(std::string payload);
};
