#include "Common.h"
#include "GlobalWeather.h"
#include "Management.h"
#include "DirectConnectedInput.h"
#include "DS18B20Interface.h"
#include "Input.h"

//cat out.txt | projects/Service.Management/bin/ARM/Debug/Service.Management.out

std::mutex management_lock;
std::mutex sensor_power_lock;
std::mutex gpio_lock;
std::string GlobalWeatherResponse;
std::atomic<time_t> reset_time;
std::map<std::string, SensorId> *sensorsTable;
DirectConnectedInput **sensors = new DirectConnectedInput*[DIRECT_SENSORS_COUNT];

std::string GetSensorIdentifier(SensorId sensorId, std::map<std::string, SensorId> *sensorsTable) {
    auto result = std::find_if(
        sensorsTable->begin(),
        sensorsTable->end(),
        [sensorId](const std::pair<std::string, SensorId>& mo) { return mo.second == sensorId; });
    if(result == sensorsTable->end()) {
        sd_journal_print(LOG_ERR, "ProcessResponce error");
    }
    return result->first;
}
void StartSensorThreads(Management *management, std::map<std::string, SensorId> *sensorsTable) {
    sensors[0] = new DirectConnectedInput(management, new DS18B20Interface(GetSensorIdentifier(DirectBoiler, sensorsTable), DirectBoiler, 1, 0));
    sensors[1] = new DirectConnectedInput(management, new DS18B20Interface(GetSensorIdentifier(DirectIndoor, sensorsTable), DirectIndoor, 1, 1.5));
    sensors[2] = new DirectConnectedInput(management, new DS18B20Interface(GetSensorIdentifier(DirectOtdoor, sensorsTable), DirectOtdoor, 1, 0.5));
    for(int i = 0; i < DIRECT_SENSORS_COUNT; i++) {
        if(sensors[i] != nullptr) {
            sensors[i]->Start();
        }
    }
}
void StopSensorThreads() {
    for(int i = 0; i < DIRECT_SENSORS_COUNT; i++) {
        if(sensors[i] != nullptr) {
            delete sensors[i];
        }
    }
}

int main(void) {
#ifdef TEST
    TestAll();
#else
    reset_time = Utils::GetTime();
    DeviceResponce deviceResponce;
    DeviceResponce prevDeviceResponce;
    Storage *storage = new Storage();
    sensorsTable = storage->ReadSensorsTable();
    GlobalWeather *gw = new GlobalWeather();
    Management *management = new Management(storage, gw);

    StartSensorThreads(management, sensorsTable);
    sd_journal_print(LOG_INFO, "Service start.");
    while(Input::Get(deviceResponce, sensorsTable)) {
        try {
            if(prevDeviceResponce == deviceResponce || deviceResponce.Sensor == Undefined) {
                continue;
            }
            prevDeviceResponce = deviceResponce;
            management->ProcessResponce(deviceResponce);
        } catch(...) {
            sd_journal_print(LOG_ERR, "ProcessResponce error");
        }
    }
    sd_journal_print(LOG_ERR, "Input stream is empty. Service stop.");
    StopSensorThreads();
    delete storage;
    delete management;
    delete sensorsTable;
    return EXIT_SUCCESS;
#endif 
}
