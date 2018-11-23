#include "Storage.h"
#include "Management.h"
using namespace std;

//cat out.txt | projects/Service.Management/bin/ARM/Debug/Service.Management.out

int main(void) {
    DeviceResponce deviceResponce;
    DeviceResponce prevDeviceResponce;
    Storage* storage = new Storage();
    Management* management = new Management();

    while(Input::Get(deviceResponce)) {
        if(prevDeviceResponce == deviceResponce) continue;
        storage->SaveResponce(deviceResponce);
        prevDeviceResponce = deviceResponce;
        management->ProcessResponce(deviceResponce);
    }

    return EXIT_SUCCESS;
}
