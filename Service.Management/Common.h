#pragma once
#include "StandartLib.h"
#include "Constants.h"
#include "CommonStruct.h"
#include "ISensorInterface.h"

extern std::mutex management_lock;
extern std::mutex sensor_power_lock;
extern std::mutex gpio_lock;
extern std::mutex log_file_lock;
extern std::string GlobalWeatherResponse;
extern std::atomic<time_t> reset_time;