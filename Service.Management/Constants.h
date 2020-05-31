#pragma once

#define HALF_LIGHT_TIME 3000
#define DEFAULT_TEMPERATURE -100
#define MAX_DELTA_DEVIATION 30000
#define MIN_CYCLE_TIME 60
#define MAX_CYCLE_TIME 2400
#define SENSOR_TIMEOUT 600
#define MAX_SENSOR_DEVIATION 8
#define DEFAULT_TIME 0
#define MAX_TEMPERATURE_DEVIATION 4
#define OUTPUT_FILE_NAME "/media/NAS/web_boiler/index.html"
#define LOG_FILE_NAME "/media/NAS/web_boiler/log.txt"
#define READ_WAIT 5
#define QUERY_INTERVAL 15
#define RESET_WAIT_INTERVAL 2
#define RESET_TIMEOUT 600
#define	W1_PREFIX	"/sys/bus/w1/devices/28-"
#define	W1_POSTFIX	"/w1_slave"
#define API_URL "https://api.openweathermap.org/data/2.5/weather?id=480562&units=metric&appid=a626320fe32f14e991091b0839149783"
#define	BOILER_STATUS_PIN 17
#define	SENSOR_POWER_PIN 27
#define TEMPLATE_FILE_NAME "/media/NAS/web_boiler/boiler_status.html"
#define SENSORS_COUNT 20
#define HOST "tcp://192.168.10.19:3307"
#define USER "boiler"
#define PASSWORD "fruPrld7uj"
#define SCHEMA "boiler"
#define CONTROL_VALUE_FILE "/media/USB/ControlValues.json"
#define SENSORS_VALUE_FILE "/media/USB/SensorValues.json"
#define SETTINGS_VALUE_FILE "/media/USB/SettingsValue.json"
#define MOSQUITTO_BROKER_ADDRESS "tcp://localhost:1883"
#define MOSQUITTO_BROKER_QOS 1