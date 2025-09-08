#include "common.hpp"
#include "backtester.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>
using namespace std;

int main(int argc, char* argv[]) {
    auto console = make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto logger = make_shared<spdlog::logger>("console", console);
    spdlog::register_logger(logger);

    if (argc < 5) {
        spdlog::error("Usage: ./backtesting <pair> <interval> <initial_cash> <strategy> [<strategy-args...>]");
        return 1;
    }

    static const vector<string> stratNames = {
        MovingAvgStrategy::getName(),
    };

    try {
        const string pair(argv[1]);
        const string interval(argv[2]);
        const double initial_cash(stod(argv[3]));
        const string strategy(argv[4]);

        if (find(stratNames.begin(), stratNames.end(), strategy) == stratNames.end()) {
            spdlog::error("You must use one of the following strategies:");
            for (const auto& s : stratNames)
                spdlog::error("        {}", s);
            return 1;
        }

        unique_ptr<Strategy> strat;
        if (strategy == MovingAvgStrategy::getName()) {
            if (argc != 7) {
                spdlog::error("Usage: ./backtesting <pair> <interval> <initial_cash> MovingAvgStrategy <short_period> <long_period>");
                return 1;
            }
            const long short_period(stol(argv[5]));
            const long long_period(stol(argv[6]));
            strat = make_unique<MovingAvgStrategy>(short_period, long_period);
        }

        DataFetcher fetcher;
        fetcher.fetchOHLC(pair, interval);
        Backtester backtester(initial_cash);
        backtester.loadData(pair, interval);
        backtester.run(strat);
    } catch (exception& e) {
        spdlog::error("Error: {}", e.what());
        return 1;
    }

    return 0;
}
