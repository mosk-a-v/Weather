#include "GlobalWeather.h"

CurrentWeather* GlobalWeather::GetWeather() {
    CurrentWeather *weather = new CurrentWeather();
    weather->IsServiceError = true;
    weather->ResponceTime = Utils::GetTime();
    try {
        if(!DownloadJSON(API_URL)) {
            return weather;
        }
        auto js = json::parse(GlobalWeatherResponse);
        auto wind = js.at("wind");
        auto main = js.at("main");
        auto clouds = js.at("clouds");
        auto sys = js.at("sys");

        weather->Clouds = clouds.at("all");
        long t;
        t = sys.at("sunrise");
        weather->SunRise = t;
        t = sys.at("sunset");
        weather->SunSet = t;
        t = js.at("dt");
        weather->LastUpdateTime = t;
        weather->TemperatureMax = main.at("temp_max");
        weather->TemperatureMin = main.at("temp_min");
        weather->TemperatureValue = main.at("temp");
        weather->WindSpeed = wind.at("speed");
        weather->IsServiceError = false;
    } catch(const std::exception &e) {
        std::stringstream ss;
        ss << "Openweathermap responce read exception." << e.what() << GlobalWeatherResponse;
        Utils::WriteLogInfo(LOG_ERR, ss.str());
    }
    return weather;
}
bool GlobalWeather::DownloadJSON(std::string URL) {
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    std::ostringstream oss;
    bool result;
    GlobalWeatherResponse.clear();
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charsets: utf-8");
    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Writer);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &GlobalWeatherResponse);
        res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        headers = NULL;
        if(CURLE_OK == res) {
            char *ct;
            res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
            if((CURLE_OK == res) && ct)
                result = true;
        } else {
            std::stringstream ss;
            ss << "Curl error" << res;
            Utils::WriteLogInfo(LOG_ERR, ss.str());
            result = false;
        }
        curl_easy_cleanup(curl);
    }
    return result;
}
int GlobalWeather::Writer(char *data, size_t size, size_t nmemb, std::string *buffer_in) {
    if(buffer_in != NULL) {
        buffer_in->append(data, size * nmemb);
        return size * nmemb;
    }
    return 0;
}
