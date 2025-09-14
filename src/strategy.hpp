#ifndef STRATEGY_HPP
#define STRATEGY_HPP

#include "common.hpp"
#include <vector>
#include <set>
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
    vector<double> open_prices_, high_prices_, low_prices_, close_prices_, volumes_;
    double findSma(long period);
    // Params for finding resistance/support levels via Zig Zag Indicators
    enum class Direction {
        None, Up, Down,
    };
    long zz_time_ = 150;    // Number of bars between 2 points to be grouped together
    double zz_diff_ = 0.05; // Max perc diff between 2 points to be grouped together
    long zz_number_ = 3;    // Min number of points to draw a resistance/support line
    set<long> zz_checked_inds_{};   // Stores which indexes have been walked thru
    void findResistanceSupport(double &level, Direction &direction);
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

class WeinsteinStrategy : public Strategy {
public:
    static const string name;
    WeinsteinStrategy(long short_period, long medium_period, long long_period,
                      long volume_avg_period,
                      double flat_slope_threshold = 0.001,
                      double volume_stage_2_breakout = 2.5, // S2: 2-3x avg_vol
                      double volume_stage_3_heavy = 5.,
                      long stage_min_period = 180);
    void updateIndicators(const OhlcDatum& datum);
    Sig genSignal(const OhlcDatum& datum);
    void resetState() {
        short_ma_ = 0.;                 medium_ma_ = 0.;                long_ma_ = 0.;
        prev_short_ma_ = 0.;            prev_medium_ma_ = 0.;           prev_long_ma_ = 0.;
        price_above_short_ma_ = false;  price_above_medium_ma_ = false; price_above_long_ma_= false;
        short_ma_slope_ = 0.;           medium_ma_slope_ = 0.;          long_ma_slope_ = 0.;
        prev_short_ma_slope_ = 0.;      prev_medium_ma_slope_ = 0.;     prev_long_ma_slope_ = 0.;
        volume_avg_ = 0.;
        resistance_ = numeric_limits<double>::max();    support_ = 0.;
        stage_ = Stage::Base;
        stage_period_ = 0;
    }
    string getName() final { return name; };
private:
    enum class Stage {
        Unknown, Base, Advancing, Distribution, Declining,
    };
    // User-specified params
    long short_period_, medium_period_, long_period_;
    long volume_avg_period_;
    double flat_slope_threshold_;       // Max slope between t-1 & t that indicates flattened SMA
    double volume_stage_2_breakout_;
    double volume_stage_3_heavy_;
    long stage_min_period_;             // How long should a stage be held before transition?
    // Internally-managed state
    bool check_buy_, check_sell_;
    double short_ma_ , medium_ma_, long_ma_;
    double prev_short_ma_, prev_medium_ma_, prev_long_ma_;
    bool price_above_short_ma_, price_above_medium_ma_, price_above_long_ma_;
    double short_ma_slope_, medium_ma_slope_, long_ma_slope_;
    double prev_short_ma_slope_, prev_medium_ma_slope_, prev_long_ma_slope_;
    double volume_avg_;
    double resistance_, support_;
    Stage stage_;
    long stage_period_;
};

#endif // STRATEGY_HPP
