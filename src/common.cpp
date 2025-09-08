#include "common.hpp"
#include <chrono>
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

