#include "common.hpp"
#include "backtester.hpp"
#include "registry.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <crow.h>
using namespace std;
using namespace boost;

int main(int argc, char* argv[]) {
    auto console = make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto logger = make_shared<spdlog::logger>("console", console);
    spdlog::register_logger(logger);

    crow::SimpleApp app;

    CROW_ROUTE(app, "/backtest")
    .methods("POST"_method)
    ([&](const crow::request& req, crow::response& res) {
        auto body = crow::json::load(req.body);
        if (!body) {
            res.code = 422;
            res.write("Invalid args");
            res.end();
            return;
        }

        try {
            // TOOD: API caller must supply the OHLCV data in the request body.
            const string pair(body["pair"].s());
            const string interval(body["interval"].s());
            const long long since(utcTime2epoch(body["since"].s()));
            if (since < 0) {
                res.code = 422;
                res.write("Invalid since");
                res.end();
                return;
            }
            const double initial_cash(stod(body["initial_cash"].s()));
            const string strategy(body["strategy"].s());

            if (find(stratNames.begin(), stratNames.end(), strategy) == stratNames.end()) {
                res.code = 422;
                res.write("Invalid strategy");
                res.end();
                return;
            }

            unique_ptr<Strategy> strat;
            if (strategy == MovingAvgStrategy::name) {
                if (!body.has("short_period") || !body.has("long_period")) {
                    res.code = 422;
                    res.write("Invalid args for " + strategy);
                    res.end();
                    return;
                }
                const long short_period(stol(body["short_period"].s()));
                const long long_period(stol(body["long_period"].s()));
                strat = make_unique<MovingAvgStrategy>(short_period, long_period);
            }
            else if (strategy == WeinsteinStrategy::name) {
                if (!body.has("short_period") || !body.has("medium_period") || !body.has("long_period") || !body.has("volume_avg_period")) {
                    res.code = 422;
                    res.write("Invalid args for " + strategy);
                    res.end();
                    return;
                }
                const long short_period(stol(body["short_period"].s()));
                const long medium_period(stol(body["medium_period"].s()));
                const long long_period(stol(body["long_period"].s()));
                const long volume_avg_period(stol(body["volume_avg_period"].s()));
                strat = make_unique<WeinsteinStrategy>(short_period, medium_period, long_period, volume_avg_period);
            }

            // TODO: Run backtest in a separate thread and return results when done.
            //       ref: https://github.com/CrowCpp/Crow/issues/258
            DataFetcher fetcher;
            // TODO: Remove fetchOHLC logic and directly use the OHLCV data supplied in the request body.
            fetcher.fetchOHLC(pair, interval, to_string(since));
            Backtester backtester(initial_cash);
            backtester.loadData(pair, interval);
            backtester.run(strat);

            res.code = 200;
            res.set_header("Content-Type", "application/json");
            // TODO: Return backtest results instead of hardcoded message.
            crow::json::wvalue x({
                {"message", "Backtest completed successfully"},
            });
            res.write(x.dump());
            res.end();
        } catch (const exception& e) {
            stringstream ss;
            ss << "Internal Server Error: " << e.what();
            res.code = 500;
            res.write(ss.str());
            res.end();
        }
    });

    app.port(8080).multithreaded().run();

    return 0;
}
