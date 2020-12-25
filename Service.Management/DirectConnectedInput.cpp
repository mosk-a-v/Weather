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
        std::stringstream ss;
        ss << "Thread for sensor " << sensor->GetSensorId() << " started.";
        Utils::WriteLogInfo(LOG_INFO, ss.str());
        while(_execute.load(std::memory_order_acquire)) {
            std::this_thread::sleep_for(std::chrono::seconds(QUERY_INTERVAL));
            try {
                DeviceResponce deviceResponce;
                if(Query(deviceResponce)) {
                    ProcessResponse(deviceResponce);
                }
            } catch(const std::exception &e) {
                std::stringstream ss;
                ss << "Thread for sensor " << sensor->GetSensorId() << " exception.";
                ss << e.what();
                Utils::WriteLogInfo(LOG_INFO, ss.str());
            }
        }
    });
}

bool DirectConnectedInput::Query(DeviceResponce& responce) {
    responce.Value = 0;
    responce.Battery = true;
    responce.Humidity = -1;
    responce.Sensor = sensor->GetSensorId();
    for(int i = 0; i < 3; i++) {
        float value = sensor->Read();
        if(value < -55 || value > 125) {
            /*std::stringstream ss;
            ss << "Wrong value for Sensor " << sensor->GetSensorId() << ". Value is `" << value << "'.";
            Utils::WriteLogInfo(LOG_INFO, ss.str());*/
            return false;
        }
        responce.Value += value / 3.0f;
        std::this_thread::sleep_for(std::chrono::seconds(READ_WAIT));
    }
    return true;
}

DirectConnectedInput::DirectConnectedInput(Management *management, ISensorInterface *sensor) : IInputInterface(management) {
    this->sensor = sensor;
}

DirectConnectedInput::DirectConnectedInput(Management* management, ISensorInterface* sensor, int queryInterval) :
    DirectConnectedInput(management, sensor) {
    this->queryInterval = queryInterval;
}

DirectConnectedInput::~DirectConnectedInput() {
    if(_execute.load(std::memory_order_acquire)) {
        Stop();
    };
    delete sensor;
}
