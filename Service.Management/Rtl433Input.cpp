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

void Rtl433Input::Connect() {
    if(client == nullptr || !client->is_connected()) {
        connOpts.set_keep_alive_interval(20);
        connOpts.set_clean_session(true);
        this->client = new mqtt::async_client(SERVER_ADDRESS, CLIENT_ID);
        client->connect(connOpts)->wait();
        client->start_consuming();
        client->subscribe(TOPIC, QOS)->wait();
    }
}

std::string Rtl433Input::ReadMessage() {
    try {
        Connect();
        auto msg = client->consume_message();
        if(!msg) {
            Disconnect();
            return "";
        }
        return msg->get_payload_str();
    } catch(std::exception &e) {
        std::stringstream ss;
        ss << "Read rtl_433 response error: " << e.what() << std::endl;
        Utils::WriteLogInfo(LOG_ERR, ss.str());
        Disconnect();
    }
    return std::string();
}

Rtl433Input::Rtl433Input(Management * management, std::map<std::string, SensorInfo> *sensorsTable) {
    this->management = management;
    this->sensorsTable = sensorsTable;
    this->client = nullptr;
}

Rtl433Input::~Rtl433Input() {
    if(_execute.load(std::memory_order_acquire)) {
        Stop();
    };
}

void Rtl433Input::Stop() {
    _execute.store(false, std::memory_order_release);
    Disconnect();
    if(_thd.joinable())
        _thd.join();
}

void Rtl433Input::Disconnect() {
    if(client != nullptr && client->is_connected()) {
        try {
            client->unsubscribe(TOPIC)->wait();
            client->stop_consuming();
            client->disconnect()->wait();
        } catch(std::exception &e) {
            std::stringstream ss;
            ss << "Disconnect error: " << e.what() << std::endl;
            Utils::WriteLogInfo(LOG_ERR, ss.str());
        }
    }
    if(client != nullptr) {
        delete client;
        client = nullptr;
    }
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
            if(ParseMessage(deviceResponce, ReadMessage())) {
                if(prevDeviceResponce == deviceResponce || deviceResponce.Sensor == Undefined) {
                    continue;
                }
                prevDeviceResponce = deviceResponce;
                management->ProcessResponce(deviceResponce);
            }
        }
    });
}

bool Rtl433Input::IsRunning() const noexcept {
    return (_execute.load(std::memory_order_acquire) && _thd.joinable());
}
