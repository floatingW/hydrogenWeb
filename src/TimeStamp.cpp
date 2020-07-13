/*
 * File: TimeStamp.cpp
 * ---------------------------------
 * @author: Fu Wei
 *
 */

#include "core/TimeStamp.hpp"

#include <tuple>

bool operator<(const TimeStamp& l, const TimeStamp& r)
{
    using namespace std::chrono;
    auto lus = duration_cast<microseconds>(l._timePoint.time_since_epoch()).count();
    auto rus = duration_cast<microseconds>(r._timePoint.time_since_epoch()).count();
    return lus < rus;
}

bool operator<=(const TimeStamp& l, const TimeStamp& r)
{
    using namespace std::chrono;
    auto lus = duration_cast<microseconds>(l._timePoint.time_since_epoch()).count();
    auto rus = duration_cast<microseconds>(r._timePoint.time_since_epoch()).count();
    return lus <= rus;
}

bool operator==(const TimeStamp& l, const TimeStamp& r)
{
    using namespace std::chrono;
    auto lus = duration_cast<microseconds>(l._timePoint.time_since_epoch()).count();
    auto rus = duration_cast<microseconds>(r._timePoint.time_since_epoch()).count();
    return lus == rus;
}

TimeStamp operator+(const TimeStamp& l, uint64_t milliSecDelay)
{
    using namespace std::chrono;
    return TimeStamp(l._timePoint + milliseconds(milliSecDelay));
}
