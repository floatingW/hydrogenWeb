/*
 * File: TimeStamp.hpp
 * -------------------
 * @author: Fu Wei
 *
 */

#ifndef HYDROGENWEB_TIMESTAMP_HPP
#define HYDROGENWEB_TIMESTAMP_HPP

#include <chrono>

class TimeStamp
{
    typedef std::chrono::system_clock::time_point TimePoint;

public:
    explicit TimeStamp(TimePoint t) :
        _timePoint(t)
    {
    }

    auto toMicroSec()
    {
        using namespace std::chrono;
        return duration_cast<microseconds>(_timePoint.time_since_epoch()).count();
    }

    auto toMilliSec()
    {
        using namespace std::chrono;
        return duration_cast<milliseconds>(_timePoint.time_since_epoch()).count();
    }

    static TimeStamp now()
    {
        using namespace std::chrono;
        return TimeStamp(system_clock::now());
    }

    friend bool operator<(const TimeStamp& l, const TimeStamp& r);
    friend bool operator==(const TimeStamp& l, const TimeStamp& r);
    friend bool operator<=(const TimeStamp& l, const TimeStamp& r);

    friend TimeStamp operator+(const TimeStamp& l, uint64_t milliSecDelay);

private:
    TimePoint _timePoint;
};

#endif //HYDROGENWEB_TIMESTAMP_HPP
