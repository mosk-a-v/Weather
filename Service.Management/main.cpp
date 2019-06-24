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

DirectConnectedInput **sensors = new DirectConnectedInput*[DIRECT_SENSORS_COUNT];

void StartSensorThreads(Management *management) {
    sensors[0] = new DirectConnectedInput(management, new DS18B20Interface(BOILER_SENSOR_ID, DirectBoiler, 1, 0));
    sensors[1] = new DirectConnectedInput(management, new DS18B20Interface(INDOOR_SENSOR_ID, DirectIndoor, 1, 0));
    sensors[2] = new DirectConnectedInput(management, new DS18B20Interface(OUTDOOR_SENSOR_ID, DirectOtdoor, 1, 0));
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
    GlobalWeather *gw = new GlobalWeather();
    Management *management = new Management(storage, gw);
    StartSensorThreads(management);
    sd_journal_print(LOG_INFO, "Service start.");
    while(Input::Get(deviceResponce)) {
        try {
            if(prevDeviceResponce == deviceResponce) {
                continue;
            }
            //storage->SaveResponce(deviceResponce);
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
    return EXIT_SUCCESS;
#endif 
}
