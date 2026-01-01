#include "common.hpp"
#include "backtester.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>
using namespace std;

int main(int argc, char* argv[]) {
    auto console = make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto logger = make_shared<spdlog::logger>("console", console);
    spdlog::register_logger(logger);

    if (argc < 6) {
        spdlog::error("Usage: ./backtesting <pair> <interval> <sinceutc-YYYYMMDDhhmmss> <initial_cash> <strategy> [<strategy-args...>]");
        return 1;
    }

    static const vector<string> stratNames = {
        MovingAvgStrategy::name,
        WeinsteinStrategy::name,
    };

    try {
        const string pair(argv[1]);
        const string interval(argv[2]);
        const long long since(utcTime2epoch(argv[3]));
        if (since < 0) {
            spdlog::error("You must specify 'since' time in YYYYMMDDhhmmss format (UTC+0, 24-hour)");
            return 1;
        }
        const double initial_cash(stod(argv[4]));
        const string strategy(argv[5]);

        if (find(stratNames.begin(), stratNames.end(), strategy) == stratNames.end()) {
            spdlog::error("You must use one of the following strategies:");
            for (const auto& s : stratNames)
                spdlog::error("        {}", s);
            return 1;
        }

        unique_ptr<Strategy> strat;
        if (strategy == MovingAvgStrategy::name) {
            if (argc != 8) {
                spdlog::error("Usage: ./backtesting <pair> <interval> <sinceutc-YYYYMMDDhhmmss> <initial_cash> %s <short_period> <long_period>", strategy);
                return 1;
            }
            const long short_period(stol(argv[6]));
            const long long_period(stol(argv[7]));
            strat = make_unique<MovingAvgStrategy>(short_period, long_period);
        }
        else if (strategy == WeinsteinStrategy::name) {
            if (argc != 10) {
                spdlog::error("Usage: ./backtesting <pair> <interval> <sinceutc-YYYYMMDDhhmmss> <initial_cash> %s <short_period> <medium_period> <long_period> <volume_avg_period>"), strategy;
                return 1;
            }
            const long short_period(stol(argv[6]));
            const long medium_period(stol(argv[7]));
            const long long_period(stol(argv[8]));
            const long volume_avg_period(stol(argv[9]));
            strat = make_unique<WeinsteinStrategy>(short_period, medium_period, long_period, volume_avg_period);
        }

        spdlog::info("Using Strategy `{}'.", strat->getName());

        DataFetcher fetcher;
        fetcher.fetchOHLC(pair, interval, to_string(since));
        Backtester backtester(initial_cash);
        backtester.loadData(pair, interval);
        backtester.run(strat);
    } catch (exception& e) {
        spdlog::error("Error: {}", e.what());
        return 1;
    }

    return 0;
}
