#ifndef STRATEGY_HPP
#define STRATEGY_HPP

#include "common.hpp"
#include <vector>
using namespace std;

enum class Sig {
    Buy,
    Sell,
    Hold
};

class MovingAvgStrategy {
    vector<double> prices_;
    int short_period_, long_period_;
    double prev_short_ma_ = 0., prev_long_ma_ = 0.;
public:
    MovingAvgStrategy(int short_period, int long_period);
    Sig genSignal(const OhlcDatum& candle);
};

#endif // STRATEGY_HPP
