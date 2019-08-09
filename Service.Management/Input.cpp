#include "Input.h"

bool Input::Get(DeviceResponce& responce, std::map<std::string, SensorId> *sensorsTable) {
    std::string inp_str;
    try {
        if(std::getline(std::cin, inp_str)) {
            auto js = json::parse(inp_str);
            int id = js.at("id");
            auto it = sensorsTable->find(std::to_string(id));
            if(it != sensorsTable->end()) {
                responce.Sensor = it->second;
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
                responce.Sensor = Undefined;
                responce.Value = DEFAULT_TEMPERATURE;
                responce.Time = "";
                responce.Humidity = -1;
                responce.Battery = false;
                return true;
            }
        } else {
            return false;
        }
    } catch(...) {
        std::string message = "Read input stream error. {" + inp_str + "}";
        sd_journal_print(LOG_ERR, message.c_str());
        responce.Sensor = Undefined;
        responce.Value = DEFAULT_TEMPERATURE;
        responce.Time = "";
        return true;
    }
}