#include "TestManagement.h"
#ifdef TEST

void TestGetRequiredIndoorTemperature() {
    TestManagement *testManagement = IninManagement();
    testManagement->SetDateTime(1, 1);
    float result = testManagement->GetRequiredIndoorTemperatureWrapper();
    if(result != 20) throw new exception();
    testManagement->SetDateTime(1, 2);
    result = testManagement->GetRequiredIndoorTemperatureWrapper();
    if(result != 19) throw new exception();
    testManagement->SetDateTime(1, 5);
    result = testManagement->GetRequiredIndoorTemperatureWrapper();
    if(result != 20.5) throw new exception();
    testManagement->SetDateTime(2, 1);
    result = testManagement->GetRequiredIndoorTemperatureWrapper();
    if(result != 19.5) throw new exception();
}
void TestGetRequiredBoilerTemperature() {
    TestManagement *testManagement = IninManagement();
    float result = testManagement->GetRequiredBoilerTemperatureWrapper(0, 20);
    if(result != 35) throw new exception();
    result = testManagement->GetRequiredBoilerTemperatureWrapper(0, 19);
    if(result != 32) throw new exception();
    result = testManagement->GetRequiredBoilerTemperatureWrapper(0, 21);
    if(result != 38) throw new exception();

    result = testManagement->GetRequiredBoilerTemperatureWrapper(-1.5, 20.3);
    if(abs(result - 37.4f) > 0.1f) throw new exception();
    result = testManagement->GetRequiredBoilerTemperatureWrapper(-2, 21);
    if(abs(result - 40.0f) > 0.1f) throw new exception();
    result = testManagement->GetRequiredBoilerTemperatureWrapper(-3, 22);
    if(abs(result - 40.0f) > 0.1f) throw new exception();
}
void TestCalclateDelta() {
    TestManagement *testManagement = IninManagement();
    testManagement->SetBoilerResponseTime(0);
    testManagement->SetBoilerTemperature(35);
    testManagement->time = 10;
    float result = testManagement->CalclateDeltaWrapper(30, 35);
    if(result != -(35 - 30) * 10 / 2) throw new exception();

    testManagement->SetBoilerResponseTime(0);
    testManagement->SetBoilerTemperature(25);
    testManagement->time = 8;
    result = testManagement->CalclateDeltaWrapper(33, 30);
    if(result != 4.5 - 12.5) throw new exception();

    testManagement->SetBoilerResponseTime(1);
    testManagement->SetBoilerTemperature(38);
    testManagement->time = 8;
    result = testManagement->CalclateDeltaWrapper(31, 33);
    if(result != 12.5 - 2) throw new exception();

    testManagement->SetBoilerResponseTime(0);
    testManagement->SetBoilerTemperature(35);
    testManagement->time = 10;
    result = testManagement->CalclateDeltaWrapper(30, 30);
    if(result != (35 - 30) * 10 / 2) throw new exception();

    testManagement->SetBoilerResponseTime(0);
    testManagement->SetBoilerTemperature(30);
    testManagement->time = 10;
    result = testManagement->CalclateDeltaWrapper(35, 35);
    if(result != -(35 - 30) * 10 / 2) throw new exception();

    testManagement->SetBoilerResponseTime(0);
    testManagement->SetBoilerTemperature(30);
    testManagement->time = 10;
    result = testManagement->CalclateDeltaWrapper(35, 30);
    if(result != (35 - 30) * 10 / 2) throw new exception();
}
void TestManageBoiler() {
    TestManagement *testManagement = IninManagement();
    testManagement->SetDateTime(1, 1);
    ProcessInput(testManagement, Outdoor, 0);
    ProcessInput(testManagement, Indoor, 20);
    testManagement->time = 1;
    ProcessInput(testManagement, Boiler, 36);
    if(testManagement->GetBoilerTemperature() != 36) throw new exception();
    if(testManagement->GetBoilerStatus() != false) throw new exception();
    testManagement->time = 4;
    ProcessInput(testManagement, Boiler, 33);
    if(testManagement->GetBoilerTemperature() != 33) throw new exception();
    if(testManagement->GetBoilerStatus() != false) throw new exception();
    testManagement->time = 5;
    ProcessInput(testManagement, Boiler, 32);
    if(testManagement->GetBoilerStatus() != false) throw new exception();
    testManagement->time = 6;
    ProcessInput(testManagement, Boiler, 34);
    if(testManagement->GetBoilerStatus() != false) throw new exception();
    testManagement->time = 7;
    ProcessInput(testManagement, Boiler, 36);
    if(testManagement->GetBoilerStatus() != false) throw new exception();
    testManagement->time = 8;
    ProcessInput(testManagement, Boiler, 38.2);
    if(testManagement->GetBoilerStatus() != false) throw new exception();
    testManagement->time = 10;
    ProcessInput(testManagement, Boiler, 36);
    if(testManagement->GetBoilerStatus() != false) throw new exception();
    testManagement->time = 14;
    ProcessInput(testManagement, Boiler, 32);
    if(testManagement->GetBoilerStatus() != false) throw new exception();
    testManagement->time = 15;
    ProcessInput(testManagement, Outdoor, -1.5);
    if(testManagement->GetBoilerStatus() != false) throw new exception();
}
void TestTemplate() {
    TestManagement *testManagement = IninManagement();
    testManagement->SetBoilerTemperature(38.3);
    testManagement->SetIndoorTemperature(20.2);
    testManagement->SetOutdoorTemperature(-5.8);
    testManagement->SetDelta(-51);
    std::stringstream ss;;
    testManagement->ApplyTemplateAndWriteWrapper(ss, 41.2, 42);
    std::string s = ss.str();
    std::cout << s << '\n';
    if(s != "Indoor: 20.2 outdoor: -5.8 boiler:38.3 requiredBoiler41.2; --42 :-51 '0' end")
        throw new exception();
}
void TestAdjustment() {
    TestManagement *testManagement = IninManagement();
    testManagement->SetIndoorTemperature(17);
    if(testManagement->GetAdjustBoilerTemperatureWrapper(40, 20) != 44) throw new exception();
    testManagement->SetIndoorTemperature(18);
    if(testManagement->GetAdjustBoilerTemperatureWrapper(40, 20) != 43) throw new exception();
    testManagement->SetIndoorTemperature(19);
    if(testManagement->GetAdjustBoilerTemperatureWrapper(40, 20) != 42) throw new exception();
    testManagement->SetIndoorTemperature(19.82);
    if(testManagement->GetAdjustBoilerTemperatureWrapper(40, 20) != 40) throw new exception();
    testManagement->SetIndoorTemperature(20.18);
    if(testManagement->GetAdjustBoilerTemperatureWrapper(40, 20) != 40) throw new exception();
    testManagement->SetIndoorTemperature(20.8);
    if(testManagement->GetAdjustBoilerTemperatureWrapper(40, 20) != 38) throw new exception();
    testManagement->SetIndoorTemperature(21);
    if(testManagement->GetAdjustBoilerTemperatureWrapper(40, 20) != 37) throw new exception();
    testManagement->SetIndoorTemperature(21.8);
    if(testManagement->GetAdjustBoilerTemperatureWrapper(40, 20) != 37) throw new exception();
    testManagement->SetIndoorTemperature(22);
    if(testManagement->GetAdjustBoilerTemperatureWrapper(40, 20) != 36) throw new exception();
}
void TestAll() {
    TestGetRequiredIndoorTemperature();
    TestGetRequiredBoilerTemperature();
    TestCalclateDelta();
    TestManageBoiler();
    TestTemplate();
    TestAdjustment();
}
void ProcessInput(Management *testManagement, SensorId sensor, float value) {
    DeviceResponce input;
    input.Sensor = sensor;
    input.Value = value;
    testManagement->ProcessResponce(input);
}
TestManagement* IninManagement() {
    std::vector<ControlValue> control;
    AddControlValue(control, 19, 0, 32);
    AddControlValue(control, 20, 0, 35);
    AddControlValue(control, 21, 0, 38);

    AddControlValue(control, 19, -1, 35);
    AddControlValue(control, 20, -1, 36);
    AddControlValue(control, 21, -1, 39);

    AddControlValue(control, 19, -2, 36);
    AddControlValue(control, 20, -2, 37);

    std::vector<SettingValue> setting;
    AddSettingValue(setting, 1, 1, 20);
    AddSettingValue(setting, 1, 2, 19);
    AddSettingValue(setting, 1, 5, 20.5);
    AddSettingValue(setting, 2, 1, 19.5);
    return new TestManagement(control, setting, "Indoor: %indoor% outdoor: %outdoor% boiler:%boiler% requiredBoiler%requiredBoiler%; --%adjustBoiler% :%delta% '%state%' end");
}
void AddControlValue(std::vector<ControlValue>& controlTable, float indoor, float outdoor, float boiler) {
    ControlValue value;
    value.Sun = 0;
    value.Wind = 0;
    value.Indoor = indoor;
    value.Outdoor = outdoor;
    value.Boiler = boiler;
    controlTable.push_back(value);
}
void AddSettingValue(std::vector<SettingValue>& settingsTable, int hour, int day, float temperature) {
    SettingValue value;
    value.WeekDay = day;
    value.Hour = hour;
    value.Temperature = temperature;
    settingsTable.push_back(value);
}

TestManagement::TestManagement(const std::vector<ControlValue>& controlTable, const std::vector<SettingValue>& settingsTable, string statusTemplate) :
    Management(controlTable, settingsTable) {
    this->statusTemplate = statusTemplate;
}
TestManagement::~TestManagement() {}
void TestManagement::SetDateTime(int hour, int wDay) {
    dateTime.tm_hour = hour;
    dateTime.tm_wday = wDay;
}
std::time_t TestManagement::GetTime() {
    return time;
}
float TestManagement::GetRequiredIndoorTemperatureWrapper() {
    return GetRequiredIndoorTemperature();
}
float TestManagement::GetRequiredBoilerTemperatureWrapper(float outdoorTemperature, float indoorTemperature) {
    return GetRequiredBoilerTemperature(0, 0, outdoorTemperature, indoorTemperature);
}
float TestManagement::CalclateDeltaWrapper(float actualilerTemperature, float requiredBoilerTemperature) {
    return CalclateDelta(actualilerTemperature, requiredBoilerTemperature, GetTime());
}
void TestManagement::SetBoilerTemperature(float temperature) {
    boilerTemperature = temperature;
}
void TestManagement::ApplyTemplateAndWriteWrapper(std::ostream &stream, float requiredBoilerTemperature, float adjustBoilerTemperature) {
    return ApplyTemplateAndWrite(stream, requiredBoilerTemperature, adjustBoilerTemperature);
}
float TestManagement::GetBoilerTemperature() {
    return boilerTemperature;
}
bool TestManagement::GetBoilerStatus() {
    return isBoilerOn;
}
void TestManagement::SetBoilerResponseTime(std::time_t time) {
    lastBoilerResponseTime = time;
}
void TestManagement::SetIndoorTemperature(float temperature) {
    indoorTemperature = temperature;
}
void TestManagement::SetOutdoorTemperature(float temperature) {
    outdoorTemperature = temperature;
}
void TestManagement::SetDelta(float value) {
    delta = value;
}
float TestManagement::GetAdjustBoilerTemperatureWrapper(float requiredBoilerTemperature, float requiredIndoorTemperature) {
    return GetAdjustBoilerTemperature(requiredBoilerTemperature, requiredIndoorTemperature);
}
std::tm* TestManagement::GetDate() {
    return &dateTime;
}

#endif // TEST