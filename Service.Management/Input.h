#pragma once
#include "Common.h"

using json = nlohmann::json;

class Input {
public:
    static bool Get(DeviceResponce& responce);
};

