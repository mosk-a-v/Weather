#pragma once
#include "mysql_connection.h"
#include <mysql_driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include "Input.h"

#define HOST "tcp://192.168.10.14:3306"
#define USER "root"
#define PASSWORD "root_root"
#define SCHEMA "Test"

class Storage {
private:
    sql::Driver *driver;
    sql::Connection *connection;
    void LogException(sql::SQLException &e);
    void Connect();
    void SaveInternal(const DeviceResponce& responce);
public:
    Storage();
    ~Storage();
    void SaveResponce(const DeviceResponce& responce);
};

