#pragma once
#include "StandartLib.h"
#include "Constants.h"
#include "CommonStruct.h"
#include "ISensorInterface.h"
#include "Utils.h"

extern std::mutex management_lock;
extern std::mutex sensor_power_lock;
extern std::mutex gpio_lock;
extern std::string GlobalWeatherResponse;


