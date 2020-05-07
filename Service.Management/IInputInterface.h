#pragma once
#include "StandartLib.h"
#include "Constants.h"
#include "Management.h"

class IInputInterface {
protected:
    std::atomic<bool> _execute;
    std::thread _thd;
    Management *management;
    void ProcessResponse(DeviceResponce &deviceResponce) {
        management->ProcessResponce(deviceResponce);
    }
    void ProcessCommand(Command *command) {
        management->ProcessCommand(command);
    }
public:
    IInputInterface(Management *management) : _execute(false) {
        this->management = management;
    }
    virtual ~IInputInterface() = default;
    bool IsRunning() const noexcept {
        return (_execute.load(std::memory_order_acquire) && _thd.joinable());
    }
    virtual void Stop() = 0;
    virtual void Start() = 0;
};