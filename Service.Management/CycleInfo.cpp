#include "CycleInfo.h"

void CycleInfo::CalculateDelta(float boilerTemperature, const time_t& now) {
    deltaForLastPeriod = CalclateDeltaForLastPeriod(boilerTemperature, now);
    delta += deltaForLastPeriod;
    if(delta < -MAX_DELTA_DEVIATION) {
        delta = -MAX_DELTA_DEVIATION;
    }
    if(delta > MAX_DELTA_DEVIATION) {
        delta = MAX_DELTA_DEVIATION;
    }
    if(abs(delta) > abs(maxDelta)) {
        maxDelta = delta;
    }
}
void CycleInfo::DetectLatency() {
    if(isLatencyPeriod) {
        return;
    }
    if(isHeating) {
        if(delta > -latency && deltaForLastPeriod > 0) {
            isLatencyPeriod = true;
        }
    } else {
        if(delta < latency && deltaForLastPeriod < 0) {
            isLatencyPeriod = true;
        }
    }
}
float CycleInfo::CalclateDeltaForLastPeriod(float boilerTemperature, const time_t& now) {
    float result = 0;
    if(lastBoilerResponceTime == DEFAULT_TIME ||
       lastBoilerTemperature == DEFAULT_TEMPERATURE ||
       requiredBoilerTemperature == DEFAULT_TEMPERATURE) {
        return result;
    }
    if(lastBoilerTemperature < boilerTemperature) {
        if(boilerTemperature <= requiredBoilerTemperature) {
            result -= (boilerTemperature - lastBoilerTemperature) * (now - lastBoilerResponceTime) / 2;
        } else if(lastBoilerTemperature < requiredBoilerTemperature && boilerTemperature > requiredBoilerTemperature) {
            float dx1 = (requiredBoilerTemperature - lastBoilerTemperature) * (now - lastBoilerResponceTime) / (boilerTemperature - lastBoilerTemperature);
            result -= dx1 * (requiredBoilerTemperature - lastBoilerTemperature) / 2;
            float dx2 = boilerTemperature - lastBoilerTemperature - dx1;
            result += dx2 * (boilerTemperature - requiredBoilerTemperature) / 2;
        } else {
            result += (boilerTemperature - lastBoilerTemperature) * (now - lastBoilerResponceTime) / 2;
        }
    }
    if(lastBoilerTemperature > boilerTemperature) {
        if(boilerTemperature >= requiredBoilerTemperature) {
            result += (lastBoilerTemperature - boilerTemperature) * (now - lastBoilerTemperature) / 2;
        } else if(boilerTemperature < requiredBoilerTemperature && lastBoilerTemperature > requiredBoilerTemperature) {
            float dx1 = (lastBoilerTemperature - requiredBoilerTemperature) * (now - lastBoilerResponceTime) / (lastBoilerTemperature - boilerTemperature);
            result += dx1 * (lastBoilerTemperature - requiredBoilerTemperature) / 2;
            float dx2 = lastBoilerTemperature - boilerTemperature - dx1;
            result -= dx2 * (requiredBoilerTemperature - boilerTemperature) / 2;
        } else {
            result -= (lastBoilerTemperature - boilerTemperature) * (now - lastBoilerResponceTime) / 2;
        }
    }
    return result;
}
void CycleInfo::EndCycle(CycleResult result, const time_t & now) {
    if(this->isCycleEnd) {
        return;
    }
    this->cycleEndTime = now;
    this->isCycleEnd = true;
    this->result = result;
}
void CycleInfo::ProcessBoilerTemperature(float value, const time_t& now) {
    if(!IsStartingMode() && !isCycleEnd) {
        CalculateDelta(value, now);
    }
    lastBoilerTemperature = value;
    lastBoilerResponceTime = now;
    DetectComplitingStartMode(now);
    if(isCycleEnd) {
        return;
    }
    if(now - cycleStartTime < MIN_CYCLE_TIME) {
        return;
    }
    if(now - cycleStartTime > MAX_CYCLE_TIME) {
        EndCycle(TimeOut, now);
        return;
    }
    if(value > (requiredBoilerTemperature + MAX_TEMPERATURE_DEVIATION) && IsBoilerOn()) {
        EndCycle(TemperatureLimit, now);
    } else if(value < (requiredBoilerTemperature - MAX_TEMPERATURE_DEVIATION) && !IsBoilerOn()) {
        EndCycle(TemperatureLimit, now);
    } else {
        if(isHeating && delta >= 0) {
            EndCycle(Normal, now);
        } else if(!isHeating && delta <= 0) {
            EndCycle(Normal, now);
        } else {
            DetectLatency();
        }
    }
}
bool CycleInfo::IsStartingMode() {
    return requiredBoilerTemperature == DEFAULT_TEMPERATURE;
}
void CycleInfo::DetectComplitingStartMode(const time_t & now) {
    if(IsStartingMode() && !isCycleEnd) {
        if(lastBoilerTemperature != DEFAULT_TEMPERATURE) {
            EndCycle(Starting, now);
        }
    }
}
bool CycleInfo::IsBoilerOn() {
    if(IsStartingMode()) {
        return false;
    }
    return (isLatencyPeriod || isCycleEnd) ? !isHeating : isHeating;
}
bool CycleInfo::IsCycleEnd() {
    return isCycleEnd;
}
CycleStatictics* CycleInfo::GetStatictics() {
    CycleStatictics *result = new CycleStatictics();
    result->BoilerRequired = requiredBoilerTemperature;
    result->CycleLength = cycleEndTime - cycleStartTime;
    result->CycleStart = cycleStartTime;
    result->IsHeating = isHeating;
    result->Result = this->result;
    result->MaxDelta = maxDelta;
    result->IsBoilerOn = IsBoilerOn();
    result->Delta = delta;
    return result;
}
CycleInfo::CycleInfo(bool isHeating, float requiredBoilerTemperature, float currentBoilerTemperature, time_t currentBoilerResponceTime, const time_t& now) {
    this->cycleStartTime = now;
    this->requiredBoilerTemperature = requiredBoilerTemperature;
    this->isHeating = isHeating;
    this->lastBoilerResponceTime = currentBoilerResponceTime;
    this->lastBoilerTemperature = currentBoilerTemperature;
}

CycleInfo::~CycleInfo() {}
