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
    if(lastBoilerResponceTime == DEFAULT_TIME ||
       lastBoilerTemperature == DEFAULT_TEMPERATURE ||
       boilerTemperature == DEFAULT_TEMPERATURE ||
       requiredBoilerTemperature == DEFAULT_TEMPERATURE) {
        return 0;
    }
    return ((lastBoilerTemperature - requiredBoilerTemperature) + (boilerTemperature - requiredBoilerTemperature)) *
        (now - lastBoilerResponceTime) / 2;
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
    if(isCycleEnd) {
        return;
    }
    CalculateDelta(value, now);
    lastBoilerTemperature = value;
    lastBoilerResponceTime = now;
    DetectComplitingStartMode(now);
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
        if(isHeating && delta > 0) {
            EndCycle(Normal, now);
        } else if(!isHeating && delta < 0) {
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
    if(IsStartingMode() && !isCycleEnd && (now - cycleStartTime) > QUERY_INTERVAL) {
        if(lastBoilerTemperature != DEFAULT_TEMPERATURE) {
            EndCycle(Starting, now);
        }
    }
}
bool CycleInfo::IsBoilerOn() {
    if(IsStartingMode()) {
        return false;
    }
    bool result = isLatencyPeriod ? !isHeating : isHeating;
    /*if(result) {
        sd_journal_print(LOG_INFO, "Boiler Is On!!!!!");
    }*/
    return result;
}
time_t CycleInfo::GetCycleLength(time_t now) {
    return (now - cycleStartTime);
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
