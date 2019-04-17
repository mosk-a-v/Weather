#include "GlobalWeather.h"

std::string GlobalWeather::Response;

bool GlobalWeather::GetWeather(CurrentWeather& weather) {
    weather.IsServiceError = false;
    weather.ResponceTime = std::time(0);
    try {
        if(!DownloadJSON(API_URL)) {
            return false;
        }
        //sd_journal_print(LOG_INFO, serviceResponce.c_str());
        auto js = json::parse(Response);

        auto wind = js.at("wind");
        auto main = js.at("main");
        auto clouds = js.at("clouds");
        auto sys = js.at("sys");

        weather.Clouds = clouds.at("all");
        long t;
        t = sys.at("sunrise");
        weather.SunRise = t;
        t = sys.at("sunset");
        weather.SunSet = t;
        t = js.at("dt");
        weather.LastUpdateTime = t;
        weather.TemperatureMax = main.at("temp_max");
        weather.TemperatureMin = main.at("temp_min");
        weather.TemperatureValue = main.at("temp");
        weather.WindSpeed = wind.at("speed");
        return true;
    } catch(const std::exception &e) {
        weather.IsServiceError = true;
        std::stringstream ss;
        ss << "Openweathermap responce read exception." << e.what();
        sd_journal_print(LOG_ERR, ss.str().c_str());
        return false;
    }
}
bool GlobalWeather::DownloadJSON(std::string URL) {
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    std::ostringstream oss;
    bool result;
    Response.clear();
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charsets: utf-8");
    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Writer);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &Response);
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
            sd_journal_print(LOG_ERR, ss.str().c_str());
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
