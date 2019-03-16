#include "Input.h"
bool Input::Get(DeviceResponce& responce) {
    string inp_str;
    try {
        if(getline(cin, inp_str)) {
            auto js = json::parse(inp_str);
            responce.Sensor = js.at("id");
            responce.Value = js.at("temperature_C");
            responce.Time = js.at("time");
            return true;
        } else {
            return false;
        }
    } catch(...) {
        cout << "Read stream error" << endl;
        return false;
    }
}