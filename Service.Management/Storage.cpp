#include "Storage.h"

Storage::Storage() {
    connection = nullptr;
}

void Storage::SaveCycleStatistics(CycleStatictics *cycleStat, SensorValues *sensorValues) {
    try {
        if(connection == nullptr) {
            Connect();
        }
        SaveCycleStatisticsInternal(cycleStat, sensorValues);
    } catch(std::exception &e) {
        LogException(e);
        delete connection;
        connection = nullptr;
    }
}

std::vector<ControlValue>* Storage::ReadControlTable() {
    std::vector<ControlValue> *result = new std::vector<ControlValue>();
    std::ifstream jsonFile;
    jsonFile.open(CONTROL_VALUE_FILE);
    nlohmann::json table;
    table << jsonFile;
    for(auto it = table.begin(); it != table.end(); ++it) {
        ControlValue value;
        value.Sun = it->at("Sun");
        value.Wind = it->at("Wind");
        value.Indoor = it->at("Indoor");
        value.Outdoor = it->at("Outdoor");
        value.Boiler = it->at("Boiler");
        result->push_back(value);
    }
    return result;
}

std::vector<SettingValue>* Storage::ReadSettingsTable() {
    std::vector<SettingValue> *result = new std::vector<SettingValue>();
    std::ifstream jsonFile;
    jsonFile.open(SETTINGS_VALUE_FILE);
    nlohmann::json table;
    table << jsonFile;
    for(auto it = table.begin(); it != table.end(); ++it) {
        SettingValue value;
        value.WeekDay = it->at("WeekDay");
        value.Hour = it->at("Hour");
        value.Temperature = it->at("Temperature");
        result->push_back(value);
    }
    return result;
}

std::map<std::string, SensorInfo>* Storage::ReadSensorsTable() {
    std::map<std::string, SensorInfo> *result = new std::map<std::string, SensorInfo>();
    std::ifstream jsonFile;
    jsonFile.open(SENSORS_VALUE_FILE);
    nlohmann::json table;
    table << jsonFile;
    for(auto it = table.begin(); it != table.end(); ++it) {
        std::string identifier = it->at("SensorIdentifier");
        int id = it->at("SensorId");
        bool useForCalc = it->at("UseForCalc");
        bool isIndoor = it->at("IsIndoor");
        bool isOutdoor = it->at("IsOutdoor");
        std::string interfaceName = it->at("Interface");
        float correctionCoefficient = it->at("CorrectionCoefficient");
        float shift = it->at("Shift");
        result->insert(std::pair<std::string, SensorInfo>(identifier, SensorInfo(id, isIndoor, isOutdoor, useForCalc, interfaceName, correctionCoefficient, shift)));
    }
    return result;
}

Storage::~Storage() {
    if(connection != nullptr) {
        delete connection;
    }
}

void Storage::LogException(std::exception &e) {
    std::stringstream ss;
    ss << "# ERR: SQLException in " << __FILE__;
    ss << "# ERR: " << e.what() << std::endl;
    Utils::WriteLogInfo(LOG_ERR, ss.str());
}

void Storage::Connect() {
    driver = get_driver_instance();
    connection = driver->connect(HOST, USER, PASSWORD);
    connection->setSchema(SCHEMA);
}

void Storage::SaveCycleStatisticsInternal(CycleStatictics * cycleStat, SensorValues *sensorValues) {
    sql::Statement *stmt = connection->createStatement();
    std::stringstream ss;
    time_t lt = cycleStat->CycleStart;
    auto local_field = *std::gmtime(&lt);
    local_field.tm_isdst = -1;
    time_t utc = std::mktime(&local_field);

    ss << "INSERT INTO `CycleStatistics` (`CycleStart`, `BoilerRequired`, `IsHeating`, `CycleLength`, `Result`, `MaxDelta`, `Delta`, " <<
        sensorValues->GetSensorColumns() << ")" <<
        " VALUES (" <<
        "FROM_UNIXTIME(" << utc << "), " <<
        cycleStat->BoilerRequired << ", " <<
        cycleStat->IsHeating << ", " <<
        cycleStat->CycleLength << ", " <<
        (short)cycleStat->Result << ", " <<
        cycleStat->MaxDelta << ", " <<
        cycleStat->Delta << ", " <<
        sensorValues->GetSensorValues() << ");";
    stmt->execute(ss.str());
    delete stmt;
}
