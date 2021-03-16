#include "DS18B20Interface.h"

void DS18B20Interface::FileOpen() {
    try {
        std::stringstream ss;
        ss << W1_PREFIX << deviceId << W1_POSTFIX;
        errno = 0;
        file = std::fopen(ss.str().c_str(), "r");
    } catch(...) {
        file = nullptr;
    }
    if(errno != 0) {
        /*std::stringstream ss;
        ss << "Sensor " << deviceId << " file open error (errno=" << errno << ").";;
        Utils::WriteLogInfo(LOG_ERR, ss.str());*/
    }
}
void DS18B20Interface::CloseFile() {
    try {
        if(file != nullptr) {
            std::fclose(file);
        }
    } catch(...) {}
    file = nullptr;
}
void DS18B20Interface::ResetSensor() {
    std::lock_guard<std::mutex> lock(sensor_power_lock);
    if(Utils::GetTime() - reset_time.load(std::memory_order_acquire) < RESET_TIMEOUT) {
        return;
    }
    reset_time.store(Utils::GetTime(), std::memory_order_release);
    Utils::SetGPIOValues(SENSOR_POWER_PIN, true);
    std::this_thread::sleep_for(std::chrono::seconds(RESET_WAIT_INTERVAL));
    Utils::SetGPIOValues(SENSOR_POWER_PIN, false);
    std::stringstream ss;
    ss << deviceId;
    Utils::WriteLogInfo(LOG_WARNING, "Device reset. Device: ", ss.str());
}
float DS18B20Interface::Read() {
    FileOpen();
    if(file == nullptr) {
        ResetSensor();
        return -9999;
    }
    float temp = -9998;
    try {
        int tempInt;
        char crcConf[5];
        fscanf(file, "%*x %*x %*x %*x %*x %*x %*x %*x %*x : crc=%*x %s", crcConf);
        if(!feof(file) && !ferror(file) && strncmp(crcConf, "YES", 3) == 0) {
            fscanf(file, "%*x %*x %*x %*x %*x %*x %*x %*x %*x t=%d", &tempInt);
            if(!ferror(file)) {
                temp = ((float)tempInt / 1000.0) * correctionCoefficient + shift;
            }
        }
    } catch(const std::exception &e) {
        std::stringstream ss;
        ss << deviceId << ". " << e.what();
        Utils::WriteLogInfo(LOG_WARNING, "Error reading from device. DeviceId and details: ", ss.str());
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
