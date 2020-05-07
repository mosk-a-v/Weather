#include "Rtl433Input.h"

bool Rtl433Input::ParseMessage(DeviceResponce& responce, std::string jsonStr) {
    responce.Sensor = Undefined;
    responce.Value = DEFAULT_TEMPERATURE;
    responce.Time = "";
    responce.Humidity = -1;
    responce.Battery = false;
    if(jsonStr.length() < 10) {
        return false;
    }
    try {
        jsonStr = jsonStr.substr(jsonStr.find('{'));
        auto js = json::parse(jsonStr);
        std::string model = js.at("model");
        bool supported = false;
        for(auto it = models.begin(); it != models.end(); ++it) {
            if(Utils::CaseInSensStringCompare(*it, model)) {
                supported = true;
                break;
            }
        }
        if(!supported) {
            return false;
        }
        int id = js.at("id");
        auto it = sensorsTable->find(std::to_string(id));
        if(it != sensorsTable->end()) {
            responce.Sensor = (it->second).Id;
            responce.Value = js.at("temperature_C");
            responce.Time = js.at("time");
            responce.Humidity = -1;
            auto battery = js.find("battery");
            auto humidity = js.find("humidity");
            if(battery != js.end()) {
                std::string status = *battery;
                responce.Battery = (status.find("OK") != std::string::npos);
            } else {
                responce.Battery = true;
            }
            if(humidity != js.end()) {
                std::string mod = js.at("model");
                if(mod != "inFactory sensor") {
                    responce.Humidity = *humidity;
                }
            }
            return true;
        } else {
            return false;
        }
    } catch(...) {
        std::string message = "Parse rtl_433 response error. {" + jsonStr + "}";
        Utils::WriteLogInfo(LOG_ERR, message);
        return false;
    }
}
Rtl433Input::Rtl433Input(Management * management, std::map<std::string, SensorInfo> *sensorsTable) : IInputInterface(management) {
    this->sensorsTable = sensorsTable;
    this->models.push_back("inFactory sensor");
    this->models.push_back("Nexus Temperature/Humidity");
}
Rtl433Input::~Rtl433Input() {
    if(_execute.load(std::memory_order_acquire)) {
        Stop();
    };
}
void Rtl433Input::Stop() {
    _execute.store(false, std::memory_order_release);
    consumer.Disconnect();
    if(_thd.joinable())
        _thd.join();
}
void Rtl433Input::Start() {
    if(_execute.load(std::memory_order_acquire)) {
        Stop();
    };
    _execute.store(true, std::memory_order_release);
    _thd = std::thread([this]() {
        std::stringstream ss;
        ss << "Thread for rtl_433 client started.";
        Utils::WriteLogInfo(LOG_INFO, ss.str());
        while(_execute.load(std::memory_order_acquire)) {
            DeviceResponce deviceResponce, prevDeviceResponce;
            if(ParseMessage(deviceResponce, consumer.ReadMessage())) {
                if(prevDeviceResponce == deviceResponce || deviceResponce.Sensor == Undefined) {
                    continue;
                }
                prevDeviceResponce = deviceResponce;
                ProcessResponse(deviceResponce);
            }
        }
    });
}
