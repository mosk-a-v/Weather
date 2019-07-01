#pragma once

#define HALF_LIGHT_TIME 3000
#define DEFAULT_TEMPERATURE -100
#define DEFAULT_LATENCY 5
#define MAX_DELTA_DEVIATION 30000
#define MIN_CYCLE_TIME 90
#define MAX_CYCLE_TIME 2000
#define SENSOR_TIMEOUT 600
#define DEFAULT_TIME 0
#define MAX_TEMPERATURE_DEVIATION 4
#define OUTPUT_FILE_NAME "/var/www/html/index.html"
#define READ_WAIT 5
#define QUERY_INTERVAL 30
#define RESET_WAIT_INTERVAL 2
#define RESET_TIMEOUT 600
#define	W1_PREFIX	"/sys/bus/w1/devices/28-"
#define	W1_POSTFIX	"/w1_slave"
#define API_URL "https://api.openweathermap.org/data/2.5/weather?id=480562&units=metric&appid=a626320fe32f14e991091b0839149783"
#define	BOILER_STATUS_PIN 17
#define	SENSOR_POWER_PIN 27
#define TEMPLATE_FILE_NAME "/var/www/html/boiler_status.html"
#define SENSORS_COUNT 20
#define DIRECT_SENSORS_COUNT 3
#define HOST "tcp://192.168.10.14:3306"
#define USER "root"
#define PASSWORD "root_root"
#define SCHEMA "weather"
#define BOILER_SENSOR_ID "030797794e9d"
#define INDOOR_SENSOR_ID "03069779b057"
#define OUTDOOR_SENSOR_ID "01144ab139aa"
