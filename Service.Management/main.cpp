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

    CurrentWeather weather;
    gw->GetWeather(weather);

    Management *management = new Management(storage->ReadControlTable(), storage->ReadSettingsTable(), gw);
    while(true) {
        if(Input::Get(deviceResponce)) {
            if(prevDeviceResponce == deviceResponce) continue;
            storage->SaveResponce(deviceResponce);
            prevDeviceResponce = deviceResponce;
            try {
                management->ProcessResponce(deviceResponce);
            } catch(...) {
                cout << "ProcessResponce error" << endl;
            }
        }
    }
    storage->~Storage();
    return EXIT_SUCCESS;
#endif
}
