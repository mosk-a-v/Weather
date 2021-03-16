#pragma once
#include "Common.h"
#include "Utils.h"
#include "Management.h"
#include "IInputInterface.h"
#include "MqttConsumer.h"
#include "Command.h"

class CommandInput : public IInputInterface {
    const std::string Topic{ "BOILER_COMMAND/JSON" };
    const std::string ClientId{ "boiler_command_consume" };

private:
    MqttConsumer consumer = MqttConsumer(Topic, ClientId);
    Command* ParseCommand(std::string jsonStr);
public:
    CommandInput(Management *management);
    ~CommandInput();
    void Stop();
    void Start();
};
