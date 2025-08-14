#include "common.hpp"
#include "backtester.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>
using namespace std;

int main(int argc, char* argv[]) {
    auto console = make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto logger = make_shared<spdlog::logger>("console", console);
    spdlog::register_logger(logger);

    if (argc != 6) {
        spdlog::error("Usage: ./backtesting <pair> <interval> <short_period> <long_period> <initial_cash>");
        return 1;
    }

    try {
        const string pair(argv[1]);
        const string interval(argv[2]);
        const long short_period(stol(argv[3]));
        const long long_period(stol(argv[4]));
        const double initial_cash(stod(argv[5]));

        DataFetcher fetcher;
        fetcher.fetchOHLC(pair, interval);
        MovingAvgStrategy strategy(short_period, long_period);

        Backtester backtester(initial_cash);
        backtester.loadData(pair, interval);
        backtester.run(strategy);
    } catch (exception& e) {
        spdlog::error("Error: {}", e.what());
        return 1;
    }

    return 0;
}
