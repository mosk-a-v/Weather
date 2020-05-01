#include "Common.h"
#include "GlobalWeather.h"
#include "Management.h"
#include "DirectConnectedInput.h"
#include "DS18B20Interface.h"
#include "Input.h"
#include "Rtl433Input.h"

//packages: git, libmysqlcppconn-dev, libcurl4-openssl-dev, nlohmann-json-dev, libsystemd-dev, wiringpi
//NFS Setup: 
//  1. NAS https://kodi.wiki/index.php?title=NFS#Synology
//  2. /etc/fstab add line '192.168.10.19:/volume1/web_boiler /media/NAS/web_boiler nfs lookupcache=all,soft,nolock,bg,nfsvers=3,ac,actimeo=1800'
//read-only: https://github.com/JasperE84/root-ro
// Mosquitto:
//  1. Install Mosquitto Broker https://randomnerdtutorials.com/how-to-install-mosquitto-broker-on-raspberry-pi/
//  2. Setup PIPE between rtl_433 & Mosquitto Broker https://tech.sid3windr.be/2017/03/getting-your-currentcost-433mhz-data-into-openhab-using-an-rtl-sdr-dongle-and-mqtt/
//  3. Installing eclipse/paho.mqtt c++ client https://unix.stackexchange.com/questions/527810/installing-eclipse-paho-mqtt-c-client-on-debian
//cat out.txt | projects/Service.Management/bin/ARM/Debug/Service.Management.out

std::mutex management_lock;
std::mutex sensor_power_lock;
std::mutex gpio_lock;
std::string GlobalWeatherResponse;
std::atomic<time_t> reset_time;
std::map<std::string, SensorInfo> *sensorsTable;
DirectConnectedInput **directSensors = new DirectConnectedInput*[DIRECT_SENSORS_COUNT];
Rtl433Input *rtlSensors;

void StartSensorThreads(Management *management, std::map<std::string, SensorInfo> *sensorsTable) {
    int i = 0;
    for(auto it = sensorsTable->begin(); it != sensorsTable->end(); ++it) {
        SensorInfo sensor = it->second;
        if(sensor.IsDirect) {
            auto sensorThread = new DirectConnectedInput(management, new DS18B20Interface(it->first, sensor.Id, sensor.CorrectionCoefficient, sensor.Shift));
            sensorThread->Start();
            directSensors[i++] = sensorThread;
        }
    }
    auto sensorThread = new Rtl433Input(management, sensorsTable);
    sensorThread->Start();
    rtlSensors = sensorThread;
}
void StopSensorThreads() {
    for(int i = 0; i < DIRECT_SENSORS_COUNT; i++) {
        if(directSensors[i] != nullptr) {
            delete directSensors[i];
        }
    }
    delete rtlSensors;
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
    Utils::WriteLogInfo(LOG_INFO, "Service start.");
    while(std::tolower(std::cin.get()) != 'q')  ;
    Utils::WriteLogInfo(LOG_ERR, "Service stop.");
    StopSensorThreads();
    delete storage;
    delete management;
    delete sensorsTable;
    return EXIT_SUCCESS;
#endif 
}
