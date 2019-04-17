#pragma once
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <systemd/sd-journal.h>

#define	W1_PREFIX	"/sys/bus/w1/devices/28-"
#define	W1_POSTFIX	"/w1_slave"

class DS18B20Interface {
    FILE *file = nullptr;
    std::string deviceId;
    void FileOpen();
public:
    float Read();
    DS18B20Interface(std::string deviceId);
    ~DS18B20Interface();
};