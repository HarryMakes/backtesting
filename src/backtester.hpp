#ifndef BACKTESTER_HPP
#define BACKTESTER_HPP

#include "common.hpp"
#include "data_fetcher.hpp"
#include "strategy.hpp"
#include <vector>
using namespace std;

class Backtester {
    vector<OhlcDatum> candles;
    double cash_, position_;
    double portfolio_value_ = 0.;
    double initial_cash_, fee_rate_;
public:
    Backtester(double initial_cash);
    void loadData(const string& csv);
    void run(MovingAvgStrategy& strategy);
};

#endif // BACKTESTER_HPP
