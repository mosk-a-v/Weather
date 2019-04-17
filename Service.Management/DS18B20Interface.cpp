#include "DS18B20Interface.h"

void DS18B20Interface::FileOpen() {
    std::stringstream ss;
    ss << W1_PREFIX << deviceId << W1_POSTFIX;
    file = fopen(ss.str().c_str(), "r");
    if(!file) {
        std::string message = "File open error . {" + ss.str() + "}";
        sd_journal_print(LOG_ERR, message.c_str());
    }
}

float DS18B20Interface::Read() {
    FileOpen();
    if(!file) {
        return -9999;
    }
    float temp = -9998;
    try{
        unsigned int tempInt;
        char crcConf[5];
        fscanf(file, "%*x %*x %*x %*x %*x %*x %*x %*x %*x : crc=%*x %s", crcConf);
        if(strncmp(crcConf, "YES", 3) == 0) {
            fscanf(file, "%*x %*x %*x %*x %*x %*x %*x %*x %*x t=%5d", &tempInt);
            temp = (float)tempInt / 1000.0;
        }
    } catch(const std::exception &e) {
        std::stringstream ss;
        ss << "Error reading from device " << deviceId << ". Detail: " << e.what();
        sd_journal_print(LOG_ERR, ss.str().c_str());
    }
    fclose(file);
    file = nullptr;
    return temp;
}

DS18B20Interface::DS18B20Interface(std::string deviceId) {
    this->deviceId = deviceId;
}

DS18B20Interface::~DS18B20Interface() {
    if(file) {
        fclose(file);
    }
}
