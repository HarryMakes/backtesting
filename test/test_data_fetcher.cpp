#include <data_fetcher.hpp>
#include <catch2/catch_all.hpp>
#include <iostream> //Debug only

TEST_CASE("DataFetcher::fetchOHLC fetches correct data for BTCUSD with interval 20", "[DataFetcher]") {
    DataFetcher dataFetcher;

    string pair{"XXBTZUSD"};
    string interval{"60"};

    dataFetcher.fetchOHLC(pair, interval, true);
    const auto json_p = dataFetcher.getJsonPtr();

    REQUIRE(json_p->size() > 0); // Ensure JSON data is not empty
    REQUIRE(json_p->contains("result"));
    REQUIRE((*json_p)["result"].contains("XXBTZUSD"));
    REQUIRE((*json_p)["result"]["XXBTZUSD"].size() > 0);

    cout << (*json_p)["result"]["XXBTZUSD"].size() << endl;
    cout << json_p->dump(2).substr(0,256) << endl;

    delete json_p;
}
