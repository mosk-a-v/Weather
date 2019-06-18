#include "DS18B20Interface.h"

void DS18B20Interface::FileOpen() {
    try {
        std::stringstream ss;
        ss << W1_PREFIX << deviceId << W1_POSTFIX;
        file = fopen(ss.str().c_str(), "r");
    } catch(...) {
        file = nullptr;
    }
}
void DS18B20Interface::CloseFile() {
    try {
        if(!file) {
            fclose(file);
        }
    } catch(...) {}
    file = nullptr;
}
void DS18B20Interface::ResetSensor() {
    std::lock_guard<std::mutex> lock(sensor_power_lock);
    Utils::SetGPIOValues(SENSOR_POWER_PIN, true);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    Utils::SetGPIOValues(SENSOR_POWER_PIN, false);
    std::stringstream ss;
    ss << "Sensor " << deviceId << " reset.";
    sd_journal_print(LOG_ERR, ss.str().c_str());
}
float DS18B20Interface::Read() {
    FileOpen();
    if(!file) {
        ResetSensor();
        return -9999;
    }
    float temp = -9998;
    try {
        unsigned int tempInt;
        char crcConf[5];
        fscanf(file, "%*x %*x %*x %*x %*x %*x %*x %*x %*x : crc=%*x %s", crcConf);
        if(strncmp(crcConf, "YES", 3) == 0) {
            fscanf(file, "%*x %*x %*x %*x %*x %*x %*x %*x %*x t=%5d", &tempInt);
            temp = ((float)tempInt / 1000.0) * correctionCoefficient + shift;
        }
    } catch(const std::exception &e) {
        std::stringstream ss;
        ss << "Error reading from device " << deviceId << ". Detail: " << e.what();
        sd_journal_print(LOG_ERR, ss.str().c_str());
        ResetSensor();
    }
    CloseFile();
    return temp;
}

DS18B20Interface::DS18B20Interface(std::string deviceId, SensorId sensorId, float correctionCoefficient, float shift) : ISensorInterface(deviceId, sensorId) {
    this->correctionCoefficient = correctionCoefficient;
    this->shift = shift;
}

DS18B20Interface::~DS18B20Interface() {
    CloseFile();
}
