#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <sstream>
#include <math.h> 
#include <curl/curl.h>
#include "nlohmann/json.hpp"
#include <systemd/sd-journal.h>
#include "CurrentWeather.h"

#define API_URL "https://api.openweathermap.org/data/2.5/weather?id=480562&units=metric&appid=a626320fe32f14e991091b0839149783"

using json = nlohmann::json;
class GlobalWeather {
    static bool DownloadJSON(std::string URL);
    static int Writer(char * data, size_t size, size_t nmemb, std::string * buffer_in);
public:
    static std::string Response;
    static bool GetWeather(CurrentWeather& weather);
};

