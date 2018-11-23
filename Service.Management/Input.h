#pragma once
#include <stdlib.h>
#include <iostream>
#include "nlohmann/json.hpp"

using json = nlohmann::json;
using namespace std;

struct DeviceResponce {
public:
    string Time;
    int Device;
    float Value;
    bool operator==(const DeviceResponce& right) {
        return Device == right.Device && Value == right.Value;
    }
};

class Input {
public:
    static bool Get(DeviceResponce& responce);
};

