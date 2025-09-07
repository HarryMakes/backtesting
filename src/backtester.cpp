#include <backtester.hpp>
#include <fstream>
using namespace std;

Backtester::Backtester(double initial_cash)
    : initial_cash_(initial_cash), cash_(initial_cash), peak_(initial_cash_), max_drawdown_(0.) {
    if (initial_cash <= 0) {
        spdlog::error("Invalid initial cash");
    }
}

void Backtester::loadData(const string& pair, const string& interval) {
    ohlc_data_.reserve(10000);
    const string infile(pair + "_" + interval + ".csv");
    ifstream inf(infile);
    if (!inf.is_open()) {
        spdlog::error("Cannot open infile {}", infile);
        return;
    }

    string line;
    getline(inf, line); // Skip CSV header
    while (getline(inf, line)) {
        stringstream ss(line);
        OhlcDatum datum;
        string token;
        getline(ss, token, ','); datum.time = stol(token);
        getline(ss, token, ','); datum.open = stod(token);
        getline(ss, token, ','); datum.high = stod(token);
        getline(ss, token, ','); datum.low = stod(token);
        getline(ss, token, ','); datum.close = stod(token);
        getline(ss, token, ','); datum.vwap = stod(token);
        getline(ss, token, ','); datum.volume = stod(token);
        getline(ss, token, ','); datum.count = stol(token);
        ohlc_data_.push_back(datum);
    }
    inf.close();
    spdlog::info("Loaded {} OHLC data for {} collected every {} minutes",
                 ohlc_data_.size(), pair, interval);
}

void Backtester::run(MovingAvgStrategy& strategy) {
    for (const auto& datum : ohlc_data_) {
        auto sig = strategy.genSignal(datum);
        executeTrade(sig, datum);
        updatePortfolio(datum);
    }
    logMetrics();
}

void Backtester::executeTrade(const Sig& sig, const OhlcDatum& datum) {
    double qty = 0;
    if (sig == Sig::Buy && cash_ > 0.) {
        // Buy as much as possible
        qty = cash_ / datum.close;
        position_ += qty;
        cash_ -= qty * datum.close;
        spdlog::info("Buy: {:.4f} Qty at ${:.2f}, t={}", qty, datum.close, datum.time);
    } else if (sig == Sig::Sell and position_ > 0.) {
        // Sell as much as possible
        qty = position_;
        cash_ += qty * datum.close;
        position_ -= qty;
        spdlog::info("Sell: {:.4f} Qty at ${:.2f}, t={}", qty, datum.close, datum.time);
    }
}

void Backtester::updatePortfolio(const OhlcDatum& datum) {
    portfolio_value_ = cash_ + position_ * datum.close;
    double ret = (portfolio_value_ - initial_cash_) / initial_cash_;
    returns_.push_back(ret);
    if (portfolio_value_ > peak_)
        peak_ = portfolio_value_;
    if ((peak_ - portfolio_value_)/peak_ > max_drawdown_)
        max_drawdown_ = (peak_ - portfolio_value_)/peak_;
}

void Backtester::logMetrics() const {
    spdlog::info("Cash: {:.2f}, Position: {:.2f}, Final Closing Price: {:.2f}",
                 cash_, position_, ohlc_data_.rbegin()->close);
    const double mean_return = accumulate(returns_.begin(), returns_.end(), 0.) / returns_.size();
    double variance = 0.;
    for (const auto& ret : returns_) {
        variance += (ret-mean_return) * (ret-mean_return);
    }
    variance /= returns_.size();
    double sharpe = variance > 0 ? (mean_return-0.01) / sqrt(variance) : 0.;    // 1% risk-free rate
    double final_return = (portfolio_value_ - initial_cash_) / initial_cash_ * 100.;
    spdlog::info("Final Portfolio Value: ${:.2f}, Returns: {:.2f}%, Sharpe Ratio: {:.2f}, Max Drawdown {:.2f}%",
                 portfolio_value_, final_return, sharpe, max_drawdown_*100.);
}
