#include "Ina219Interface.h"

Ina219Interface::Ina219Interface(std::string deviceId, SensorId sensorId, float correctionCoefficient, float shift) : ISensorInterface(deviceId, sensorId) {
    this->correctionCoefficient = correctionCoefficient;
    this->shift = shift;
    int i2cAddress = std::stoi(deviceId, 0, 16);
    this->i2cFd = Utils::SetupI2C(i2cAddress);
    if(!init()) {
        this->i2cFd = -1;
    }
    setADCMode(SAMPLE_MODE_128);
    setMeasureMode(CONTINUOUS);
    setPGain(PG_40);
    setBusRange(BRNG_16);
}

float Ina219Interface::Read() {
    float value = getBusVoltage_V() * correctionCoefficient + shift;
    if(value < 0.140) {
        std::stringstream ss;
        ss << "Sensor " << deviceId << " value error (value=" << value << ").";;
        Utils::WriteLogInfo(LOG_ERR, ss.str());
    }
    return value;
}

bool Ina219Interface::init() {
    if (!reset_INA219()) {
        return false;
    }
    setADCMode(BIT_MODE_12);
    setMeasureMode(CONTINUOUS);
    setPGain(PG_320);
    setBusRange(BRNG_32);
    calc_overflow = false;
    return true;
}

bool Ina219Interface::reset_INA219() {
    uint16_t ack = writeRegister(INA219_CONF_REG, INA219_RST);
    return ack == 0;
}

void Ina219Interface::setCorrectionFactor(float corr) {
    calValCorrected = calVal * corr;
    writeRegister(INA219_CAL_REG, calValCorrected);
}

void Ina219Interface::setADCMode(INA219_ADC_MODE mode) {
    deviceADCMode = mode;
    uint16_t currentConfReg = readRegister(INA219_CONF_REG);
    currentConfReg &= ~(0x0780);
    currentConfReg &= ~(0x0078);
    uint16_t adcMask = mode << 3;
    currentConfReg |= adcMask;
    adcMask = mode << 7;
    currentConfReg |= adcMask;
    writeRegister(INA219_CONF_REG, currentConfReg);
}

void Ina219Interface::setMeasureMode(INA219_MEASURE_MODE mode) {
    deviceMeasureMode = mode;
    uint16_t currentConfReg = readRegister(INA219_CONF_REG);
    currentConfReg &= ~(0x0007);
    currentConfReg |= deviceMeasureMode;
    writeRegister(INA219_CONF_REG, currentConfReg);
}

void Ina219Interface::setPGain(INA219_PGAIN gain) {
    devicePGain = gain;
    uint16_t currentConfReg = readRegister(INA219_CONF_REG);
    currentConfReg &= ~(0x1800);
    currentConfReg |= devicePGain;
    writeRegister(INA219_CONF_REG, currentConfReg);

    switch (devicePGain) {
    case PG_40:
        calVal = 20480;
        currentDivider_mA = 50.0;
        pwrMultiplier_mW = 0.4;
        break;
    case PG_80:
        calVal = 10240;
        currentDivider_mA = 25.0;
        pwrMultiplier_mW = 0.8;
        break;
    case PG_160:
        calVal = 8192;
        currentDivider_mA = 20.0;
        pwrMultiplier_mW = 1.0;
        break;
    case PG_320:
        calVal = 4096;
        currentDivider_mA = 10.0;
        pwrMultiplier_mW = 2.0;
        break;
    }
    writeRegister(INA219_CAL_REG, calVal);
}

void Ina219Interface::setBusRange(INA219_BUS_RANGE range) {
    deviceBusRange = range;
    uint16_t currentConfReg = readRegister(INA219_CONF_REG);
    currentConfReg &= ~(0x2000);
    currentConfReg |= deviceBusRange;
    writeRegister(INA219_CONF_REG, currentConfReg);
}

float Ina219Interface::getShuntVoltage_mV() {
    int16_t val;
    val = (int16_t)readRegister(INA219_SHUNT_REG);
    return (val * 0.01);
}


float Ina219Interface::getBusVoltage_V() {
    uint16_t val;
    val = readRegister(INA219_BUS_REG);
    val = ((val >> 3) * 4);
    return (val * 0.001);
}


float Ina219Interface::getCurrent_mA() {
    int16_t val;
    val = (int16_t)readRegister(INA219_CURRENT_REG);
    return (val / currentDivider_mA);
}


float Ina219Interface::getBusPower() {
    uint16_t val;
    val = readRegister(INA219_PWR_REG);
    return (val * pwrMultiplier_mW);
}

bool Ina219Interface::getOverflow() {
    uint16_t val;
    val = readRegister(INA219_BUS_REG);
    bool ovf = (val & 1);
    return ovf;
}


void Ina219Interface::startSingleMeasurement() {
    uint16_t val = readRegister(INA219_BUS_REG); // clears CNVR (Conversion Ready) Flag
    val = readRegister(INA219_CONF_REG);
    writeRegister(INA219_CONF_REG, val);
    uint16_t convReady = 0x0000;
    while (!convReady) {
        convReady = ((readRegister(INA219_BUS_REG)) & 0x0002); // checks if sampling is completed
    }
}

void Ina219Interface::powerDown() {
    confRegCopy = readRegister(INA219_CONF_REG);
    setMeasureMode(POWER_DOWN);
}

void Ina219Interface::powerUp() {
    writeRegister(INA219_CONF_REG, confRegCopy);
    std::this_thread::sleep_for(std::chrono::microseconds(40));
}

uint16_t Ina219Interface::writeRegister(uint8_t reg, uint16_t val) {
    if(i2cFd < 0) {
        return 0;
    }
    uint8_t lVal = val & 255;
    uint8_t hVal = val >> 8;
    return Utils::WriteToI2C(i2cFd, reg, (lVal << 8) + hVal);
}

uint16_t Ina219Interface::readRegister(uint8_t reg) {
    if(i2cFd < 0) {
        return 0;
    }
    uint16_t val = Utils::ReadFromI2C(i2cFd, reg);
    uint8_t lVal = val & 255;
    uint8_t hVal = val >> 8;

    uint16_t regValue = lVal;
    return (regValue << 8) | hVal;
}
