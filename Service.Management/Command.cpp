#include "Command.h"

const std::string DirectText{ "direct" };

Command::Command(std::string text, float value) {
    this->text = text;
    this->value = value;
}

TemperatureStrategyBase* Command::GetTemperatureStrategy() {
    if(Utils::CaseInSensStringCompare(text, DirectText)) {
        return new DirectTemperatureStrategy(value);
    } else {
        return new NormalTemperatureStrategy();
    }
}
