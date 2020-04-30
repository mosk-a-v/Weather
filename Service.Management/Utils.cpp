#include "Utils.h"

float Utils::GetAdjustBoilerTemperature(float indoorTemperature, float requiredIndoorTemperature, float requiredBoilerTemperature) {
    if(indoorTemperature < requiredIndoorTemperature - 0.6)
        return requiredBoilerTemperature + 8;
    else if(indoorTemperature < requiredIndoorTemperature - 0.4)
        return requiredBoilerTemperature + 6;
    else if(indoorTemperature < requiredIndoorTemperature - 0.2)
        return requiredBoilerTemperature + 4;
    else if(indoorTemperature < requiredIndoorTemperature - 0.05)
        return requiredBoilerTemperature + 2;
    else if(indoorTemperature < requiredIndoorTemperature + 0.05)
        return requiredBoilerTemperature;
    else if(indoorTemperature < requiredIndoorTemperature + 0.2)
        return requiredBoilerTemperature - 2;
    else if(indoorTemperature < requiredIndoorTemperature + 0.4)
        return requiredBoilerTemperature - 4;
    else if(indoorTemperature < requiredIndoorTemperature + 0.6)
        return requiredBoilerTemperature - 6;
    return requiredBoilerTemperature - 8;
}

std::string Utils::FormatTime(const time_t & time) {
    std::stringstream buffer;
    tm tm = *localtime(&time);
    buffer.imbue(std::locale("ru_RU.utf8"));
    buffer << std::put_time(&tm, "%T");
    return buffer.str();
}

std::string Utils::FormatDateTime(const time_t & time) {
    std::stringstream buffer;
    tm tm = *localtime(&time);
    buffer.imbue(std::locale("ru_RU.utf8"));
    buffer << std::put_time(&tm, "%F %T");
    return buffer.str();
}

float Utils::GetSunAdjust(int sun) {
    if(sun < 30)
        return 0;
    else if(sun < 50)
        return 1;
    else if(sun < 70)
        return 2;
    else if(sun < 80)
        return 3;
    else if(sun < 90)
        return 4;
    else
        return 5;
}

float Utils::GetWindAdjust(int wind) {
    if(wind < 10)
        return 0;
    else if(wind < 20)
        return 2;
    else if(wind < 30)
        return 3;
    else
        return 4;
}

void Utils::SetupGPIO() {
    wiringPiSetupGpio();
    pinMode(BOILER_STATUS_PIN, OUTPUT);
    pinMode(SENSOR_POWER_PIN, OUTPUT);
    digitalWrite(BOILER_STATUS_PIN, true);
    digitalWrite(SENSOR_POWER_PIN, false);
}

void Utils::SetGPIOValues(int pin, bool value) {
    //std::lock_guard<std::mutex> lock(gpio_lock);
    digitalWrite(pin, value);
}

std::time_t Utils::GetTime() {
    return std::time(0);
}

void Utils::WriteLogInfo(int priority, std::string message) {
    sd_journal_print(priority, message.c_str());
    try {
        std::ofstream statusStream;
        statusStream.open(LOG_FILE_NAME, std::ofstream::out | std::ofstream::app);
        std::string prefix;
        switch(priority) {
            case LOG_EMERG:
                prefix = "EMERGENCY";
                break;
            case LOG_ALERT:
                prefix = "ALERT";
                break;
            case LOG_CRIT:
                prefix = "CRITICAL";
                break;
            case LOG_ERR:
                prefix = "ERROR";
                break;
            case LOG_WARNING:
                prefix = "WARNING";
                break;
            case LOG_NOTICE:
                prefix = "NOTICE";
                break;
            case LOG_INFO:
                prefix = "INFO";
                break;
            case LOG_DEBUG:
                prefix = "DEBUG";
                break;
            default:
                prefix = "N|/A";
                break;
        }
        statusStream << FormatDateTime(GetTime()) << ". " << prefix << ": " << message<< std::endl;
        statusStream.close();
    } catch(std::exception e) {
        Utils::WriteLogInfo(LOG_ERR, "Status write exception.");
    }
}

std::string Utils::ReadFile(std::string fileName) {
    try {
        std::ifstream fileStream;
        fileStream.open(fileName);
        std::stringstream buffer;
        buffer << fileStream.rdbuf();
        fileStream.close();
        return buffer.str();
    } catch(const std::exception &e) {
        std::stringstream ss;
        ss << "File '" << fileName << "' read exception." << e.what();
        Utils::WriteLogInfo(LOG_ERR, ss.str());
        return "";
    }
}
