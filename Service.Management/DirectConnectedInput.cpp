#include "DirectConnectedInput.h"

void DirectConnectedInput::Stop() {
    _execute.store(false, std::memory_order_release);
    if(_thd.joinable())
        _thd.join();
}

void DirectConnectedInput::Start(int interval, Management *management) {
    if(_execute.load(std::memory_order_acquire)) {
        Stop();
    };
    _execute.store(true, std::memory_order_release);
    _thd = std::thread([this, interval, management]() {
        sd_journal_print(LOG_INFO, "Thread for DirectConnectedInput started.");
        while(_execute.load(std::memory_order_acquire)) {
            try {
                DeviceResponce deviceResponce;
                if(Query(deviceResponce)) {
                    management->ProcessResponce(deviceResponce);
                }
                std::this_thread::sleep_for(std::chrono::seconds(interval));
            } catch(...) {
                globalExceptionPtr = std::current_exception();
                break;
            }
        }
    });
}

bool DirectConnectedInput::IsRunning() const noexcept {
    return (_execute.load(std::memory_order_acquire) && _thd.joinable());
}

bool DirectConnectedInput::Query(DeviceResponce& responce) {
    float value = boilerSensor->Read();
    std::this_thread::sleep_for(std::chrono::seconds(READ_WAIT));
    value = boilerSensor->Read();
    responce.Sensor = DirectBoiler;
    responce.Value = value;
    return value > -9000;
}

DirectConnectedInput::DirectConnectedInput() :_execute(false) {
    boilerSensor = new DS18B20Interface(SENSOR_ID);
}

DirectConnectedInput::~DirectConnectedInput() {
    if(_execute.load(std::memory_order_acquire)) {
        Stop();
        boilerSensor->~DS18B20Interface();
    };
}
