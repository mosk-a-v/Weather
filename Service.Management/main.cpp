#include "Management.h"
#include "TestManagement.h"
using namespace std;

//cat out.txt | projects/Service.Management/bin/ARM/Debug/Service.Management.out

int main(void) {
    
#ifdef TEST
    TestAll();
#else
    freopen("/home/kotel/output.log", "w", stderr);
    DeviceResponce deviceResponce;
    DeviceResponce prevDeviceResponce;
    Storage *storage = new Storage();
    Management *management = new Management(storage->ReadControlTable(), storage->ReadSettingsTable());
    while(Input::Get(deviceResponce)) {
        if(prevDeviceResponce == deviceResponce) continue;
        storage->SaveResponce(deviceResponce);
        prevDeviceResponce = deviceResponce;
        management->ProcessResponce(deviceResponce);
    }
    return EXIT_SUCCESS;
#endif
}
