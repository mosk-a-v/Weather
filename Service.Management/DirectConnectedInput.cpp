#include "DirectConnectedInput.h"

void DirectConnectedInput::Stop() {
    _execute.store(false, std::memory_order_release);
    if(_thd.joinable())
        _thd.join();
}

void DirectConnectedInput::Start() {
    if(_execute.load(std::memory_order_acquire)) {
        Stop();
    };
    _execute.store(true, std::memory_order_release);
    _thd = std::thread([this]() {
        sd_journal_print(LOG_INFO, "Thread for DirectConnectedInput started.");
        while(_execute.load(std::memory_order_acquire)) {
            try {
                DeviceResponce deviceResponce;
                if(Query(deviceResponce)) {
                    management->ProcessResponce(deviceResponce);
                } else {
                    std::stringstream ss;
                    ss << "DirectConnectedInput. Sensor " << sensor->GetSensorId() << " value error.";
                    sd_journal_print(LOG_ERR, ss.str().c_str());
                }
                std::this_thread::sleep_for(std::chrono::seconds(QUERY_INTERVAL));
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
    responce.Value = 0;
    responce.Sensor = sensor->GetSensorId();
    for(int i = 0; i < 3; i++) {
        float value;
        int retry = 0;
        do {
            std::this_thread::sleep_for(std::chrono::seconds(READ_WAIT));
            value = sensor->Read();
            retry++;
        } while((value < 0 || value > 85) && retry < 3);
        if(retry >= 3) {
            return false;
        }
        responce.Value += value / 3.0f;
    }
    return true;
}

DirectConnectedInput::DirectConnectedInput(Management *management, ISensorInterface *sensor) :_execute(false) {
    this->sensor = sensor;
    this->management = management;
}

DirectConnectedInput::~DirectConnectedInput() {
    if(_execute.load(std::memory_order_acquire)) {
        Stop();
    };
}
