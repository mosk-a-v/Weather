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
                return true;
            } else {
                responce.Sensor = Undefined;
                responce.Value = DEFAULT_TEMPERATURE;
                responce.Time = "";
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