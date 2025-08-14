#include "data_fetcher.hpp"
#include <curl/curl.h>
#include <fstream>

void DataFetcher::fetchOHLC(const string& pair, const string& interval, bool store_json) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        spdlog::error("Cannot init curl");
        return;
    }
    string url = "https://api.kraken.com/0/public/OHLC?pair=" + pair + "&interval=" + interval;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    string response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode ret = curl_easy_perform(curl);
    if (ret != CURLE_OK) {
        spdlog::error("Curl returns {}", ret);
    }
    curl_easy_cleanup(curl);

    auto json = nlohmann::json::parse(response);
    if (json.contains("error") && !json["error"].empty()) {
        spdlog::warn("JSON API responded with error {}", json["error"].dump());
    } else {
        saveToCsv(json, pair, interval);
    }
    if (store_json) {
        if (json_p != nullptr) {
            delete json_p;
        }
        json_p = new nlohmann::json(json);      // Copy constructor
    }
}

size_t DataFetcher::writeCallback(void* contents, size_t size, size_t nmemb, string* data) {
    data->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

void DataFetcher::saveToCsv(const nlohmann::json& json, const string& pair, const string& interval) {
    const string outfile(pair + "_" + interval + ".csv");
    ofstream outf(outfile);
    if (!outf.is_open()) {
        spdlog::error("Cannot open outfile {}", outfile);
        return;
    }
    outf << "Time,Open,High,Low,Close,VWAP,Volume,Count" << endl;
    for (const auto& datum : json["result"].front()) {
        outf << datum[0].get<long>() << "," << datum[1].get<string>() << ","
             << datum[2].get<string>() << "," << datum[3].get<string>() << ","
             << datum[4].get<string>() << "," << datum[5].get<string>() << ","
             << datum[6].get<string>() << "," << datum[7].get<long>() << endl;
    }
    outf.close();
    spdlog::info("Data written to outfile {}", outfile);
}
