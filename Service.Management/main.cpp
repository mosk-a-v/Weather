#include "Common.h"
#include "GlobalWeather.h"
#include "Management.h"
#include "DirectConnectedInput.h"
#include "DS18B20Interface.h"
#include "CommandInput.h"
#include "Rtl433Input.h"
#include "Ina219Interface.h"

//packages: git, libmysqlcppconn-dev, libcurl4-openssl-dev, nlohmann-json-dev, libsystemd-dev, wiringpi, libi2c-dev
//NFS Setup: 
//  1. NAS https://kodi.wiki/index.php?title=NFS#Synology
//  2. /etc/fstab add line '192.168.10.19:/volume1/web_boiler /media/NAS/web_boiler nfs lookupcache=all,soft,nolock,bg,nfsvers=3,ac,actimeo=1800'
//read-only: https://github.com/JasperE84/root-ro
// Mosquitto:
//  1. Install Mosquitto Broker https://randomnerdtutorials.com/how-to-install-mosquitto-broker-on-raspberry-pi/
//  2. Setup PIPE between rtl_433 & Mosquitto Broker https://tech.sid3windr.be/2017/03/getting-your-currentcost-433mhz-data-into-openhab-using-an-rtl-sdr-dongle-and-mqtt/
//  3. Installing eclipse/paho.mqtt c++ client https://unix.stackexchange.com/questions/527810/installing-eclipse-paho-mqtt-c-client-on-debian
// mosquitto_pub -h 192.168.10.14 -t BOILER_COMMAND/JSON -m "{\"Command\":\"Direct\", \"Value\":29}"
// rtl_433 -U -C si -R 91 -R 19 -F json 2>/dev/null  | mosquitto_pub -h 192.168.10.14 -i RTL_433 -l -t RTL_433/JSON

std::mutex management_lock;
std::mutex sensor_power_lock;
std::mutex log_file_lock;
std::mutex gpio_lock;
std::string GlobalWeatherResponse;
std::atomic<time_t> reset_time;
std::vector<IInputInterface*> inputThreads;
std::map<std::string, SensorInfo> *sensorsTable;

void StartInputThreads(Management* management, std::map<std::string, SensorInfo>* sensorsTable) {
    for(auto it = sensorsTable->begin(); it != sensorsTable->end(); ++it) {
        SensorInfo sensor = it->second;
        DirectConnectedInput* sensorThread = nullptr;
        if(sensor.InterfaceName == "1WIRE") {
            sensorThread = new DirectConnectedInput(management, new DS18B20Interface(it->first, sensor.Id, sensor.CorrectionCoefficient, sensor.Shift));
            sensorThread->Start();
        }
        else if(sensor.InterfaceName == "I2C") {
            sensorThread = new DirectConnectedInput(management, new Ina219Interface(it->first, sensor.Id, sensor.CorrectionCoefficient, sensor.Shift), 5);
            sensorThread->Start();
        }
        if(sensorThread != nullptr) {
            inputThreads.push_back(sensorThread);
        }
    }
    auto rtlThread = new Rtl433Input(management, sensorsTable);
    rtlThread->Start();
    inputThreads.push_back(rtlThread);

    auto commandThread = new CommandInput(management);
    commandThread->Start();
    inputThreads.push_back(commandThread);
}
void StopInputThreads() {
    for(auto it = inputThreads.begin(); it != inputThreads.end(); ++it) {
        if(*it != nullptr) {
            delete *it;
        }
    }
}
bool IsInputThreadsRunning() {
    for(auto it = inputThreads.begin(); it != inputThreads.end(); ++it) {
        if(*it == nullptr || !((*it)->IsRunning())) {
            return false;
        }
    }
    return true;
}

int main(void) {
    reset_time = Utils::GetTime();
    Storage *storage = new Storage();
    GlobalWeather *gw = new GlobalWeather();
    Management *management = new Management(storage, gw);

    sensorsTable = Storage::ReadSensorsTable();
    StartInputThreads(management, sensorsTable);
    Utils::WriteLogInfo(LOG_INFO, "Service start.");
    while(true) {
        std::this_thread::sleep_for(std::chrono::seconds(60));
        if(!IsInputThreadsRunning()) {
            break;
        }
    }
    Utils::WriteLogInfo(LOG_ERR, "Service stop.");
    StopInputThreads();
    delete storage;
    delete management;
    delete sensorsTable;
    return EXIT_SUCCESS;
}
