#include "Storage.h"

Storage::Storage() {
    Connect();
}

void Storage::SaveCycleStatistics(CycleStatictics *cycleStat, SensorValues *sensorValues) {
    try {
        SaveCycleStatisticsInternal(cycleStat, sensorValues);
    } catch(std::exception &e) {
        LogException(e);
        delete connection;
        Connect();
    }
}

std::vector<ControlValue>* Storage::ReadControlTable() {
    std::vector<ControlValue> *result = new std::vector<ControlValue>();
    sql::Statement *stmt = connection->createStatement();
    sql::ResultSet *resultSet = stmt->executeQuery("SELECT Sun, Wind, Indoor, Outdoor, Boiler FROM ControlTable;");
    while(resultSet->next()) {
        ControlValue value;
        value.Sun = resultSet->getInt("Sun");
        value.Wind = resultSet->getInt("Wind");
        value.Indoor = resultSet->getDouble("Indoor");
        value.Outdoor = resultSet->getDouble("Outdoor");
        value.Boiler = resultSet->getDouble("Boiler");
        result->push_back(value);
    }
    delete resultSet;
    delete stmt;
    return result;
}

std::vector<SettingValue>* Storage::ReadSettingsTable() {
    std::vector<SettingValue> *result = new std::vector<SettingValue>();
    sql::Statement *stmt = connection->createStatement();
    sql::ResultSet *resultSet = stmt->executeQuery("SELECT WeekDay, Hour, Temperature FROM TemperatureSettings;");
    while(resultSet->next()) {
        SettingValue value;
        value.WeekDay = resultSet->getInt("WeekDay");
        value.Hour = resultSet->getInt("Hour");
        value.Temperature = resultSet->getDouble("Temperature");
        result->push_back(value);
    }
    delete resultSet;
    delete stmt;
    return result;
}

std::map<std::string, SensorId>* Storage::ReadSensorsTable() {
    std::map<std::string, SensorId> *result = new std::map<std::string, SensorId>();
    sql::Statement *stmt = connection->createStatement();
    sql::ResultSet *resultSet = stmt->executeQuery("SELECT SensorId, SensorIdentifier FROM SensorTable;");
    while(resultSet->next()) {
        SensorId id = (SensorId)(resultSet->getInt("SensorId"));
        std::string identifier = (resultSet->getString("SensorIdentifier")).asStdString();
        result->insert(std::pair<std::string, SensorId>(identifier, id));
    }
    delete resultSet;
    delete stmt;
    return result;
}

Storage::~Storage() {
    delete connection;
}

void Storage::LogException(std::exception &e) {
    std::stringstream ss;
    ss << "# ERR: SQLException in " << __FILE__;
    ss << "# ERR: " << e.what() << std::endl;
    sd_journal_print(LOG_ERR, ss.str().c_str());
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
