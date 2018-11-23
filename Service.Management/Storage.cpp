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
        << ", " << responce.Device 
        << ");";
    stmt->execute(ss.str());
}
