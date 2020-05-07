#pragma once
#include "Common.h"
#include "TemperatureStrategyBase.h"
#include "DirectTemperatureStrategy.h"
#include "NormalTemperatureStrategy.h"

class Command {
private:
    std::string text;
    float value;

public:
    Command(std::string text, float value);
    TemperatureStrategyBase* GetTemperatureStrategy();
};

