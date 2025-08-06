#include <data_fetcher.hpp>
#include <catch2/catch_all.hpp>
#include <iostream> //Debug only

TEST_CASE("DataFetcher::fetchOHLC fetches correct data for BTCUSD with interval 20", "[DataFetcher]") {
    DataFetcher dataFetcher;

    string pair{"BTCUSD"};
    string interval{"60"};

    dataFetcher.fetchOHLC(pair, interval);

    cout << dataFetcher.json.size() << endl;
    cout << dataFetcher.json.dump(2) << endl;

    REQUIRE(dataFetcher.json.size() > 0); // Ensure JSON data is not empty
}
