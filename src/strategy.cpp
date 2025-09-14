/*#define IS_DEBUG // DEBUG*/

#include "strategy.hpp"
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include "zzindic/zigzag.h"
#ifdef __cplusplus
}
#endif // __cplusplus
#include <numeric>
#ifdef IS_DEBUG
#include <iostream>
#endif // IS_DEBUG
#include <cstdlib>
using namespace std;

const string Strategy::name = "base";
const string MovingAvgStrategy::name = "moving_avg";
const string WeinsteinStrategy::name = "weinstein";

double Strategy::findSma(long period) {
    // Find the SMA value on the last read day.
    auto start = close_prices_.end() - period;    // Itor
    return accumulate(start, close_prices_.end(), 0.) / period;
}

void Strategy::findResistanceSupport(double &level, Direction &direction) {
    if (high_prices_.size() < 2)
        return;

    // (Ref: https://github.com/BatuhanUsluel/Algorithmic-Support-and-Resistance)
    direction = Direction::None;
    long zz_len = high_prices_.size();
    double zz_change = 1.0;
    long zz_percent = 0;
    long zz_retrace = 0;
    long zz_lex = 0;
    vector<double> zz_pivots(zz_len);
    vector<long> zz_sigs(zz_len);

    zigzag(&high_prices_[0], &low_prices_[0], &zz_len, &zz_change, &zz_percent, &zz_retrace, &zz_lex,
           &zz_pivots[0], &zz_sigs[0]);
    vector<long> curr_checked_inds{};
    vector<double> curr_checked_pivots{};
    
    for (long ind = 0; ind < zz_pivots.size(); ++ind) {
        if (!zz_checked_inds_.contains(ind)) {
            curr_checked_inds.clear();
            curr_checked_inds.push_back(ind);
            long counter = 0;
            curr_checked_pivots.clear();
            curr_checked_pivots.push_back(zz_pivots[ind]);
            auto startx = ind, endx = ind;
            auto dir = zz_sigs[ind];
            for (long ind2 = 0; ind2 < zz_pivots.size(); ++ind2) {
                if (!zz_checked_inds_.contains(ind2)) {
                    if (ind != ind2 && abs(ind2-ind) < zz_time_ && zz_sigs[ind2] == dir) {
                        if (abs((zz_pivots[ind]/zz_pivots[ind2])-1)<(zz_diff_/100)) {
                            curr_checked_inds.push_back(ind2);
                            curr_checked_pivots.push_back(zz_pivots[ind2]);
                            if (ind2 < startx)
                                startx = ind2;
                            else if (ind2 > endx)
                                endx = ind2;
                            counter = counter + 1;
                        }
                    }
                }
            }
            if (counter > zz_number_) {
                level = accumulate(curr_checked_pivots.begin(), curr_checked_pivots.end(), 0.)/curr_checked_pivots.size();
                if (dir == 1)
                    direction = Direction::Up;
                else if (dir == -1)
                    direction = Direction::Down;
                for (const auto& di : curr_checked_inds)
                    zz_checked_inds_.insert(di);
            }
        }
    }
}

MovingAvgStrategy::MovingAvgStrategy(long short_period, long long_period)
    : short_period_(short_period), long_period_(long_period) {
    resetState();
    if (short_period <= 0 || long_period <= 0 || short_period >= long_period) {
        spdlog::error("Invalid Moving Avg periods");
    }
}

void MovingAvgStrategy::updateIndicators(const OhlcDatum& datum) {
    const long curr_ind = close_prices_.size();

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
    close_prices_.push_back(datum.close);

    Sig sig = Sig::Hold;
    // If long_period has not passed, just hold.
    if (close_prices_.size() < long_period_) {
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

WeinsteinStrategy::WeinsteinStrategy(long short_period, long medium_period, long long_period,
                                     long volume_avg_period,
                                     double flat_slope_threshold,
                                     double volume_stage_2_breakout,
                                     double volume_stage_3_heavy,
                                     long stage_min_period)
    : short_period_(short_period), medium_period_(medium_period), long_period_(long_period),
      volume_avg_period_(volume_avg_period),
      flat_slope_threshold_(flat_slope_threshold),
      volume_stage_2_breakout_(volume_stage_2_breakout),
      volume_stage_3_heavy_(volume_stage_3_heavy),
      stage_min_period_(stage_min_period) {
    resetState();
    if (short_period <= 0 || medium_period <= 0 || long_period <= 0 ||
        !(short_period <= medium_period && medium_period <= long_period)) {
        spdlog::error("Invalid Moving Avg periods");
    }
    if (volume_avg_period <= 0) {
        spdlog::error("Invalid Volume Avg period");
    }
}

void WeinsteinStrategy::updateIndicators(const OhlcDatum& datum) {
    const long curr_ind = close_prices_.size();

    short_ma_ = findSma(short_period_);
    medium_ma_ = findSma(medium_period_);
    long_ma_ = findSma(long_period_);
#ifdef IS_DEBUG
    cout << "- [" << datum.time << "] $" << datum.close << ": short_ma_=$" << short_ma_ << ": medium_ma_=$" << medium_ma_ << " long_ma_=$" << long_ma_ << endl;
#endif // IS_DEBUG

    // Find Volumn Avg, independent of the SMA periods
    auto volume_avg_start_ = volumes_.size() >= volume_avg_period_ ? 
                             volumes_.begin() : volumes_.end()-volume_avg_period_;
    volume_avg_ = accumulate(volume_avg_start_, volumes_.end(), 0.) /
                  distance(volume_avg_start_, volumes_.end());

    // Update resistance/support
    double rs_level = 0.;
    Direction rs_direction = Direction::None;
    findResistanceSupport(rs_level, rs_direction);
    if (rs_direction == Direction::Up)
        support_ = rs_level;
    else if (rs_direction == Direction::Down)
        resistance_ = rs_level;

    // Misc stats on how the current price holds in comparison to other attributes
    if (!price_above_short_ma_ && datum.close > short_ma_)
        price_above_short_ma_ = true;
    else if (price_above_short_ma_ && datum.close <= short_ma_)
        price_above_short_ma_ = false;
    if (!price_above_medium_ma_ && datum.close > medium_ma_)
        price_above_medium_ma_ = true;
    else if (price_above_medium_ma_ && datum.close <= medium_ma_)
        price_above_medium_ma_ = false;
    if (!price_above_long_ma_ && datum.close > long_ma_)
        price_above_long_ma_ = true;
    else if (price_above_long_ma_ && datum.close <= long_ma_)
        price_above_long_ma_ = false;    
}

Sig WeinsteinStrategy::genSignal(const OhlcDatum& datum) {
    // Generate signal on the last read day.
    close_prices_.push_back(datum.close);
    high_prices_.push_back(datum.high);
    low_prices_.push_back(datum.low);
    volumes_.push_back(datum.volume);

    Sig sig = Sig::Hold;
    // If long_period has not passed, just hold.
    if (close_prices_.size() < long_period_) {
        return sig;
    }

    updateIndicators(datum);

    bool is_flat_short = abs(short_ma_slope_) < flat_slope_threshold_;
    bool is_flat_medium = abs(medium_ma_slope_) < flat_slope_threshold_;
    bool is_flat_long = abs(long_ma_slope_) < flat_slope_threshold_;
    bool is_volume_stage_2_breakout = datum.volume > volume_stage_2_breakout_ * volume_avg_;
    bool is_volume_stage_3_heavy = datum.volume > volume_stage_3_heavy_ * volume_avg_;

    Stage new_stage = stage_;

    // Stage 1: Sideways, flattening MAs, low initial volume
    if (datum.close > support_ && datum.close < resistance_ && is_flat_medium && is_flat_long) {
        new_stage = Stage::Base;
    }
    // Stage 2: Breakout above resistance & volume, persistently above short/medium/long MAs, rising short MA
    else if (datum.close > resistance_ && is_volume_stage_2_breakout &&
             price_above_short_ma_ && price_above_medium_ma_ && price_above_long_ma_ &&
             short_ma_slope_ > 0) {
        new_stage = Stage::Advancing;
    }
    // Stage 3: Erratic, below short MA on high vol, flattening medium/long MAs
    else if (datum.close < short_ma_ && is_volume_stage_3_heavy &&
             is_flat_medium && is_flat_long) {
        new_stage = Stage::Distribution;
    }
    // Stage 4: Breaks below support, goes medium/long MAs, dropping medium/long MAs
    else if (datum.close < support_ && datum.close < medium_ma_ && datum.close < long_ma_ &&
             medium_ma_slope_ < 0 && long_ma_slope_ < 0) {
        new_stage = Stage::Declining;
    }

    // Generate signal on stage transition
    if (stage_ == Stage::Unknown || (new_stage != stage_ && stage_period_ >= stage_min_period_)) {
        if (new_stage == Stage::Advancing)
            sig = Sig::Buy;
        else if (new_stage == Stage::Distribution || new_stage == Stage::Declining) {
            sig = Sig::Sell;
        }
        stage_ = new_stage;
        stage_period_ = 0;
    } else {
        ++stage_period_;
    }

    return sig;
}