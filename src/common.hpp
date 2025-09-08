#ifndef COMMON_HPP
#define COMMON_HPP

#include <vector>
#include <spdlog/spdlog.h>
using namespace std;

struct OhlcDatum {
    long time;
    double open, high, low, close, vwap, volume;
    long count;
};

string epoch2localTime(const long long& epoch);

#endif // COMMON_HPP
