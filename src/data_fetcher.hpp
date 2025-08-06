#ifndef DATA_FETCHER_HPP
#define DATA_FETCHER_HPP

#include <string>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
using namespace std;

class DataFetcher {
public:
    DataFetcher() = default;
    ~DataFetcher() {}

    void fetchOHLC(const string& pair, const string& interval);

    nlohmann::json json; //for initial skeleton code

private:
    vector<string> data;
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, string* data);
};

#endif // DATA_FETCHER_HPP