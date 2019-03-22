#include "Management.h"
#include "TestManagement.h"
using namespace std;

//cat out.txt | projects/Service.Management/bin/ARM/Debug/Service.Management.out
std::string GlobalWeather::Response;

int main(void) {
#ifdef TEST
    TestAll();
#else
    DeviceResponce deviceResponce;
    DeviceResponce prevDeviceResponce;
    Storage *storage = new Storage();
    GlobalWeather *gw = new GlobalWeather();
    Management *management = new Management(storage, gw);
    sd_journal_print(LOG_INFO, "Service start.");
    while(Input::Get(deviceResponce)) {
        if(prevDeviceResponce == deviceResponce) continue;
        storage->SaveResponce(deviceResponce);
        prevDeviceResponce = deviceResponce;
        try {
            management->ProcessResponce(deviceResponce);
        } catch(...) {
            sd_journal_print(LOG_ERR, "ProcessResponce error");
        }
    }
    sd_journal_print(LOG_ERR, "Input stream is empty. Service stop.");
    storage->~Storage();
    return EXIT_SUCCESS;
#endif 
}
