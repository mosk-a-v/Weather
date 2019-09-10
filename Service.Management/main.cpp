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
std::map<std::string, SensorInfo> *sensorsTable;
DirectConnectedInput **sensors = new DirectConnectedInput*[DIRECT_SENSORS_COUNT];

void StartSensorThreads(Management *management, std::map<std::string, SensorInfo> *sensorsTable) {
    int i = 0;
    for(auto it = sensorsTable->begin(); it != sensorsTable->end(); ++it) {
        SensorInfo sensor = it->second;
        if(sensor.IsDirect) {
            auto sensorThread = new DirectConnectedInput(management, new DS18B20Interface(it->first, sensor.Id, sensor.CorrectionCoefficient, sensor.Shift));
            sensorThread->Start();
            sensors[i++] = sensorThread;
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
    Management *management = new Management(storage, gw, sensorsTable);

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
