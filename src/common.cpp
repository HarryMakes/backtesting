#include "common.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
using namespace std;

string epoch2localTime(const long long& epoch) {
    chrono::system_clock::time_point tp = chrono::system_clock::from_time_t(epoch);
    time_t time_t_val = chrono::system_clock::to_time_t(tp);
    tm* local_tm = localtime(&time_t_val);
    if (!local_tm)
        return "<Invalid timestamp>";
    stringstream ss;
    ss << put_time(local_tm, "%F %T%z");
    return ss.str();
}

long long utcTime2epoch(const string& utc_time) {
    tm utc_tm = {
        .tm_sec = stoi(utc_time.substr(12, 2)),
        .tm_min = stoi(utc_time.substr(10, 2)),
        .tm_hour = stoi(utc_time.substr(8, 2)),
        .tm_mday = stoi(utc_time.substr(6, 2)),
        .tm_mon = stoi(utc_time.substr(4, 2)) - 1,
        .tm_year = stoi(utc_time.substr(0, 4)) - 1900,
    };
    return mktime(&utc_tm);
}
