#include "Common.h"
#include "GlobalWeather.h"
#include "Management.h"
#include "DirectConnectedInput.h"
#include "DS18B20Interface.h"
#include "Input.h"

//cat out.txt | projects/Service.Management/bin/ARM/Debug/Service.Management.out

int main(void) {
#ifdef TEST
    TestAll();
#else
    DeviceResponce deviceResponce;
    DeviceResponce prevDeviceResponce;
    Storage *storage = new Storage();
    GlobalWeather *gw = new GlobalWeather();
    Management *management = new Management(storage, gw);
    ISensorInterface *boilerSensor = new DS18B20Interface(BOILER_SENSOR_ID, DirectBoiler);
    ISensorInterface *indoorSensor = new DS18B20Interface(INDOOR_SENSOR_ID, DirectIndoor);
    DirectConnectedInput *boilerQuery = new DirectConnectedInput(management, boilerSensor);
    boilerQuery->Start();
    DirectConnectedInput *indoorQuery = new DirectConnectedInput(management, indoorSensor);
    indoorQuery->Start();
    sd_journal_print(LOG_INFO, "Service start.");
    while(Input::Get(deviceResponce)) {
        if(globalExceptionPtr) {
            try {
                std::rethrow_exception(globalExceptionPtr);
            } catch(const std::exception &ex) {
                std::stringstream message_stream;
                message_stream << "Thread for query DirectConnected device exited with exception: " << ex.what() << std::endl;
                sd_journal_print(LOG_ERR, message_stream.str().c_str());
                globalExceptionPtr = nullptr;
                boilerQuery->~DirectConnectedInput();
                boilerQuery = new DirectConnectedInput(management, boilerSensor);
                boilerQuery->Start();
                indoorQuery->~DirectConnectedInput();
                indoorQuery = new DirectConnectedInput(management, boilerSensor);
                indoorQuery->Start();
            }
        }
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
    boilerQuery->~DirectConnectedInput();
    indoorQuery->~DirectConnectedInput();
    storage->~Storage();
    return EXIT_SUCCESS;
#endif 
}
