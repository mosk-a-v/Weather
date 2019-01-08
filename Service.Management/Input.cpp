#include "Input.h"
bool Input::Get(DeviceResponce& responce) {
    string inp_str;
    if(getline(cin, inp_str)) {
        auto js = json::parse(inp_str);
        responce.Sensor = js.at("id");
        responce.Value = js.at("temperature_C");
        responce.Time = js.at("time");
    } else {
        return false;
    }
}