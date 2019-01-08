#include "Storage.h"

Storage::Storage() {
    Connect();
}

void Storage::SaveResponce(const DeviceResponce& responce) {
    try {
        SaveInternal(responce);
    } catch(sql::SQLException &e) {
        LogException(e);
        delete connection;
        Connect();
    }
}

std::vector<ControlValue> Storage::ReadControlTable() {
    std::vector<ControlValue> result;
    sql::Statement *stmt = connection->createStatement();
    sql::ResultSet *resultSet = stmt->executeQuery("SELECT Sun, Wind, Indoor, Outdoor, Boiler FROM ControlTable;");
    while(resultSet->next()) {
        ControlValue value;
        value.Sun = resultSet->getInt("Sun");
        value.Wind = resultSet->getInt("Wind");
        value.Indoor = resultSet->getDouble("Indoor");
        value.Outdoor = resultSet->getDouble("Outdoor");
        value.Boiler = resultSet->getDouble("Boiler");
        result.push_back(value);
    }
    delete resultSet;
    delete stmt;
    return result;
}

std::vector<SettingValue> Storage::ReadSettingsTable() {
    std::vector<SettingValue> result;
    sql::Statement *stmt = connection->createStatement();
    sql::ResultSet *resultSet = stmt->executeQuery("SELECT WeekDay, Hour, Temperature FROM TemperatureSettings;");
    while(resultSet->next()) {
        SettingValue value;
        value.WeekDay = resultSet->getInt("WeekDay");
        value.Hour = resultSet->getInt("Hour");
        value.Temperature = resultSet->getDouble("Temperature");
        result.push_back(value);
    }
    delete resultSet;
    delete stmt;
    return result;
}

Storage::~Storage() {
    delete connection;
}

void Storage::LogException(sql::SQLException &e) {
    cout << "# ERR: SQLException in " << __FILE__;
    cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
    cout << "# ERR: " << e.what();
    cout << " (MySQL error code: " << e.getErrorCode();
    cout << ", SQLState: " << e.getSQLState() << " )" << endl;
}

void Storage::Connect() {
    driver = get_driver_instance();
    connection = driver->connect(HOST, USER, PASSWORD);
    connection->setSchema(SCHEMA);
}

void Storage::SaveInternal(const DeviceResponce & responce) {
    sql::Statement *stmt = connection->createStatement();
    stringstream ss;
    ss << "INSERT INTO `Temperature` (`Time`, `Value`, `Device`) VALUES ('" << responce.Time
        << "', " << responce.Value
        << ", " << (int)responce.Sensor
        << ");";
    stmt->execute(ss.str());
    delete stmt;
}
