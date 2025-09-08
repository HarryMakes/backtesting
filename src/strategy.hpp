#ifndef STRATEGY_HPP
#define STRATEGY_HPP

#include "common.hpp"
#include <vector>
using namespace std;

enum class Sig {
    Buy,
    Sell,
    Hold,
};

class Strategy {
public:
    virtual ~Strategy() = default;
    virtual void updateIndicators(const OhlcDatum& datum) = 0;
    virtual Sig genSignal(const OhlcDatum& datum) = 0;
    virtual void resetState() {};
    static string getName() { return "BaseStrategy"; };
};

class MovingAvgStrategy : public Strategy {
    vector<double> prices_, volumes_;
    bool check_buy_ = false, check_sell_ = false;
    long short_period_, long_period_;
    double short_ma_ = 0., long_ma_ = 0.;
    double prev_short_ma_ = 0., prev_long_ma_ = 0.;
    long golden_cross_inds_[2] = {-1,-1}, death_cross_inds_[2] = {-1,-1};
    double max_closing_price_ = 0., min_closing_price_ = 0.;
public:
    MovingAvgStrategy(long short_period, long long_period);
    void updateIndicators(const OhlcDatum& datum);
    Sig genSignal(const OhlcDatum& datum);
    void resetState() {
        prices_.clear();
        volumes_.clear();
        short_period_ = 0;
        long_period_ = 0;
        short_ma_ = 0.;
        long_ma_ = 0.;
        prev_short_ma_ = 0.;
        prev_long_ma_ = 0.;
        for (int i = 0; i < 2; ++i) {
            golden_cross_inds_[i] = -1;
            death_cross_inds_[i] = -1;
        }
        max_closing_price_ = 0.;
        min_closing_price_ = 0.;
    }
    static string getName() { return "MovingAvgStrategy"; };
private:
    double findSma(long period);
};

#endif // STRATEGY_HPP
