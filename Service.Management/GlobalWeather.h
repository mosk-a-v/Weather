#pragma once
#include "Common.h"
#include "Utils.h"

using json = nlohmann::json;
class GlobalWeather {
    static bool DownloadJSON(std::string URL);
    static int Writer(char * data, size_t size, size_t nmemb, std::string * buffer_in);
public:
    static CurrentWeather* GetWeather();
};

