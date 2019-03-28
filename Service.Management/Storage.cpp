#include "Storage.h"

Storage::Storage() {
    Connect();
}

void Storage::SaveResponce(const DeviceResponce& responce) {
    try {
        SaveResponceInternal(responce);
    } catch(sql::SQLException &e) {
        LogException(e);
        delete connection;
        Connect();
    }
}

void Storage::SaveCycleStatistics(CycleStatictics *cycleStat) {
    try {
        SaveCycleStatisticsInternal(cycleStat);
    } catch(sql::SQLException &e) {
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

Storage::~Storage() {
    delete connection;
}

void Storage::LogException(sql::SQLException &e) {
    stringstream ss;
    ss << "# ERR: SQLException in " << __FILE__;
    ss << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
    ss << "# ERR: " << e.what();
    ss << " (MySQL error code: " << e.getErrorCode();
    ss << ", SQLState: " << e.getSQLState() << " )" << endl;
    sd_journal_print(LOG_ERR, ss.str().c_str());
}

void Storage::Connect() {
    driver = get_driver_instance();
    connection = driver->connect(HOST, USER, PASSWORD);
    connection->setSchema(SCHEMA);
}

void Storage::SaveResponceInternal(const DeviceResponce & responce) {
    sql::Statement *stmt = connection->createStatement();
    stringstream ss;
    ss << "INSERT INTO `Temperature` (`Time`, `Value`, `Device`) VALUES ('" <<
        responce.Time << "', " <<
        responce.Value << ", " <<
        (int)responce.Sensor << ");";
    stmt->execute(ss.str());
    delete stmt;
}

void Storage::SaveCycleStatisticsInternal(CycleStatictics * cycleStat) {
    sql::Statement *stmt = connection->createStatement();
    stringstream ss;
    time_t lt = cycleStat->CycleStart;
    auto local_field = *std::gmtime(&lt);
    local_field.tm_isdst = -1;
    time_t utc = std::mktime(&local_field);

    ss << "INSERT INTO `CycleStatistics` (`CycleStart`, `AvgIndoor`, `AvgOutdoor`, `AvgBoiler`, `Wind`, `Sun`, `BoilerRequired`, " <<
        "`IsHeating`, `CycleLength`, `LastIndoor`, `LastOutdoor`, `LastBoiler`, `Result`)" <<
        " VALUES (" <<
        "FROM_UNIXTIME(" << utc << "), " <<
        cycleStat->AvgIndoor << ", " <<
        cycleStat->AvgOutdoor << ", " <<
        cycleStat->AvgBoiler << ", " <<
        cycleStat->Wind << ", " <<
        cycleStat->Sun << ", " <<
        cycleStat->BoilerRequired << ", " <<
        cycleStat->IsHeating << ", " <<
        cycleStat->CycleLength << ", " <<
        cycleStat->LastIndoor << ", " <<
        cycleStat->LastOutdoor << ", " <<
        cycleStat->LastBoiler << ", " <<
        (short)cycleStat->Result << ");";
    stmt->execute(ss.str());
    delete stmt;
}
