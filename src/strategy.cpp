/*#define IS_DEBUG // DEBUG*/

#include "strategy.hpp"
#include <numeric>
#ifdef IS_DEBUG
#include <iostream>
#endif // IS_DEBUG
using namespace std;

const string Strategy::name = "base";
const string MovingAvgStrategy::name = "moving_avg";

double Strategy::findSma(long period) {
    // Find the SMA value on the last read day.
    auto start = prices_.end() - period;    // Itor
    return accumulate(start, prices_.end(), 0.) / period;
}

MovingAvgStrategy::MovingAvgStrategy(long short_period, long long_period)
    : short_period_(short_period), long_period_(long_period) {
    resetState();
    if (short_period <= 0 || long_period <= 0 || short_period >= long_period) {
        spdlog::error("Invalid Moving Avg periods");
    }
}

void MovingAvgStrategy::updateIndicators(const OhlcDatum& datum) {
    const long curr_ind = prices_.size();

    short_ma_ = findSma(short_period_);
    long_ma_ = findSma(long_period_);
#ifdef IS_DEBUG
    cout << "- [" << datum.time << "] $" << datum.close << ": short_ma_=$" << short_ma_ << " long_ma_=$" << long_ma_ << endl;
#endif // IS_DEBUG

    // Tb1 = Current golden cross
    // Tb2 = Current death cross
    // Tb3 = Next golden cross
    // Ts1 = Current death cross
    // Ts2 = Current golden cross
    // Ts3 = Next death cross
    if (short_ma_ > long_ma_ && prev_short_ma_ < prev_long_ma_) {
        golden_cross_inds_[0] = golden_cross_inds_[1];
        golden_cross_inds_[1] = curr_ind;
        // If this is Tb3 where (Tb3-Tb2)<(Tb2-Tb1)/2, then check high price to Buy.
        if (golden_cross_inds_[0] >= 0 && death_cross_inds_[1] >= 0) {
            if (golden_cross_inds_[1]-death_cross_inds_[1] < (death_cross_inds_[1]-golden_cross_inds_[0])/2)
                check_buy_ = true;
            else
                check_buy_ = false;
        }
    }
    else if (short_ma_ < long_ma_ && prev_short_ma_ > prev_long_ma_) {
        death_cross_inds_[0] = death_cross_inds_[1];
        death_cross_inds_[1] = curr_ind;
        // If this is Ts3 where (Ts3-Ts2)<(Ts2-Ts1)/2, then check low price to Sell.
        if (death_cross_inds_[0] >= 0 && golden_cross_inds_[1] >= 0) {
            if (death_cross_inds_[1]-golden_cross_inds_[1] < (golden_cross_inds_[1]-death_cross_inds_[0])/2)
                check_sell_ = true;
            else
                check_sell_ = false;
        }
    }
    prev_short_ma_ = short_ma_;
    prev_long_ma_ = long_ma_;

    // Find the max_closing_price between Tb1 and Tb2.
    if (death_cross_inds_[1] - golden_cross_inds_[1] > 0 && datum.close > max_closing_price_) {
        max_closing_price_ = datum.close;
    }
    // Find the min_closing_price between Ts1 and Ts2.
    if (golden_cross_inds_[1] - death_cross_inds_[1] > 0 && datum.close < min_closing_price_) {
        min_closing_price_ = datum.close;
    }
}

Sig MovingAvgStrategy::genSignal(const OhlcDatum& datum) {
    // Generate signal on the last read day.
    prices_.push_back(datum.close);

    Sig sig = Sig::Hold;
    // If long_period has not passed, just hold.
    if (prices_.size() < long_period_) {
        return sig;
    }

    updateIndicators(datum);

    // If price reaches max_closing_price after Tb3, then Buy.
    if (check_buy_ && datum.open >= max_closing_price_) {
        sig = Sig::Buy;
        check_buy_ = false;
    }
    // If price reaches min_closing_price after Ts3, then Sell.
    if (check_sell_ && datum.open <= min_closing_price_) {
        sig = Sig::Sell;
        check_sell_ = false;
    }

    return sig;
}
