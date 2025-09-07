#ifndef BACKTESTER_HPP
#define BACKTESTER_HPP

#include "common.hpp"
#include "data_fetcher.hpp"
#include "strategy.hpp"
#include <vector>
using namespace std;

class Backtester {
    vector<OhlcDatum> ohlc_data_;
    vector<double> returns_;
    double cash_, position_ = 0.;
    double portfolio_value_ = 0.;
    double initial_cash_, fee_rate_;
    double peak_, max_drawdown_;
public:
    Backtester(double initial_cash);
    void loadData(const string& pair, const string& interval);
    void run(MovingAvgStrategy& strategy);
private:
    void executeTrade(const Sig& sig, const OhlcDatum& datum);
    void updatePortfolio(const OhlcDatum& datum);
    void logMetrics() const;
};

#endif // BACKTESTER_HPP
