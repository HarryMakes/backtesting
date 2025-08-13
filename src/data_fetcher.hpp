#ifndef DATA_FETCHER_HPP
#define DATA_FETCHER_HPP

#include "common.hpp"
#include <string>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
using namespace std;

class DataFetcher {
public:
    DataFetcher() = default;
    ~DataFetcher() {}

    void fetchOHLC(const string& pair, const string& interval, bool save_json = false);

    nlohmann::json* getJsonPtr() const {
        return json_p;
    }

private:
    nlohmann::json* json_p = nullptr;
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, string* data);
    void saveToCsv(const nlohmann::json& json, const string& outfile);
};

#endif // DATA_FETCHER_HPP