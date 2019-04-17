#include<exception>
#include<stdexcept>
#include "Management.h"
#include "DirectConnectedInput.h"
#include "TestManagement.h"
using namespace std;

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
    DirectConnectedInput *dcQuery = new DirectConnectedInput();
    dcQuery->Start(30, management);
    sd_journal_print(LOG_INFO, "Service start.");
    while(Input::Get(deviceResponce)) {
        if(globalExceptionPtr) {
            try {
                std::rethrow_exception(globalExceptionPtr);
            } catch(const std::exception &ex) {
                stringstream message_stream;
                message_stream << "Thread for query DirectConnected device exited with exception: " << ex.what() << endl;
                sd_journal_print(LOG_ERR, message_stream.str().c_str());
                globalExceptionPtr = nullptr;
                dcQuery->~DirectConnectedInput();
                dcQuery = new DirectConnectedInput();
                dcQuery->Start(30, management);
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
    dcQuery->~DirectConnectedInput();
    storage->~Storage();
    return EXIT_SUCCESS;
#endif 
}
