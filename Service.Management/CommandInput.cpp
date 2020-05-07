#include "CommandInput.h"

Command* CommandInput::ParseCommand(std::string jsonStr) {
    try {
        jsonStr = jsonStr.substr(jsonStr.find('{'));
        auto js = json::parse(jsonStr);
        std::string text = js.at("Command");
        float value = js.at("Value");
        return new Command(text, value);
    } catch(...) {
        std::string message = "Parse command response error. {" + jsonStr + "}";
        Utils::WriteLogInfo(LOG_ERR, message);
        return nullptr;
    }
}
CommandInput::CommandInput(Management * management) : IInputInterface(management) {

}
CommandInput::~CommandInput() {
    if(_execute.load(std::memory_order_acquire)) {
        Stop();
    };
}
void CommandInput::Stop() {
    _execute.store(false, std::memory_order_release);
    consumer.Disconnect();
    if(_thd.joinable())
        _thd.join();
}
void CommandInput::Start() {
    if(_execute.load(std::memory_order_acquire)) {
        Stop();
    };
    _execute.store(true, std::memory_order_release);
    _thd = std::thread([this]() {
        std::stringstream ss;
        ss << "Thread for command client started.";
        Utils::WriteLogInfo(LOG_INFO, ss.str());
        while(_execute.load(std::memory_order_acquire)) {
            auto command = ParseCommand(consumer.ReadMessage());
            if(command != nullptr) {
                ProcessCommand(command);
                delete command;
            }
        }
    });
}
