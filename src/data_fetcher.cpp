#include "data_fetcher.hpp"
#include <curl/curl.h>
#include <fstream>

void DataFetcher::fetchOHLC(const string& pair, const string& interval) {
    CURL* curl = curl_easy_init();
    if (curl) {
        string url = "https://api.kraken.com/0/public/OHLC?pair=" + pair + "&interval=" + interval;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        CURLcode ret = curl_easy_perform(curl);
        if (ret == CURLE_OK) {
            json = nlohmann::json::parse(response);
        }
    }
    curl_easy_cleanup(curl);
}

size_t DataFetcher::writeCallback(void* contents, size_t size, size_t nmemb, string* data) {
    data->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}
