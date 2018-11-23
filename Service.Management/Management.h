#include <wiringPi.h>
#include "Input.h"
#define	PIN	17

#pragma once
class Management {
public:
    void ProcessResponce(const DeviceResponce& responce);
    Management();
    ~Management();
};

