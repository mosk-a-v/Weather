#pragma once
#include <stdlib.h>
#include <iostream>
#include "nlohmann/json.hpp"
#include <systemd/sd-journal.h>
#include "GlobalWeather.h"

using json = nlohmann::json;
using namespace std;

enum SensorId {
    Outdoor = 113,
    Indoor = 150,
    Boiler = 179,
    DirectBoiler = 180
};

struct DeviceResponce {
public:
    string Time;
    SensorId Sensor;
    float Value;
    bool operator==(const DeviceResponce& right) {
        return Sensor == right.Sensor && Value == right.Value;
    }
};

class Input {
public:
    static bool Get(DeviceResponce& responce);
};

