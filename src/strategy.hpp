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
    static const string name;
    virtual ~Strategy() = default;
    virtual void updateIndicators(const OhlcDatum& datum) = 0;
    virtual Sig genSignal(const OhlcDatum& datum) = 0;
    virtual void resetState() {};
    virtual string getName() = 0;
protected:
    vector<double> prices_, volumes_;
    double findSma(long period);
};

class MovingAvgStrategy : public Strategy {
public:
    static const string name;
    MovingAvgStrategy(long short_period, long long_period);
    void updateIndicators(const OhlcDatum& datum);
    Sig genSignal(const OhlcDatum& datum);
    void resetState() {
        check_buy_ = false;     check_sell_ = false;
        short_ma_ = 0.;         long_ma_ = 0.;
        prev_short_ma_ = 0.;    prev_long_ma_ = 0.;
        for (int i = 0; i < 2; ++i) {
            golden_cross_inds_[i] = -1;
            death_cross_inds_[i] = -1;
        }
        max_closing_price_ = 0.;
        min_closing_price_ = 0.;
    }
    string getName() final { return name; };
private:
    // User-specified params
    long short_period_, long_period_;
    // Internally-managed state
    bool check_buy_, check_sell_;
    double short_ma_, long_ma_;
    double prev_short_ma_, prev_long_ma_;
    long golden_cross_inds_[2], death_cross_inds_[2];
    double max_closing_price_, min_closing_price_;
};


#endif // STRATEGY_HPP
