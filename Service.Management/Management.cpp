#include "Management.h"

Management::Management() {
    wiringPiSetupSys();
}

void Management::ProcessResponce(const DeviceResponce & responce) {
    digitalWrite(PIN, HIGH);
}

Management::~Management() {}
