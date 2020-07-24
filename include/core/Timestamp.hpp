/*
 * File: Timestamp.hpp
 * -------------------
 * @author: Fu Wei
 *
 */

#ifndef HYDROGENWEB_TIMESTAMP_HPP
#define HYDROGENWEB_TIMESTAMP_HPP

#include <chrono>
#include <string>

class Timestamp
{
    typedef std::chrono::system_clock::time_point TimePoint;

public:
    Timestamp() = default;

    explicit Timestamp(TimePoint t) :
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

    auto toSec()
    {
        using namespace std::chrono;
        return duration_cast<seconds>(_timePoint.time_since_epoch()).count();
    }

    std::string toString();

    static Timestamp now()
    {
        using namespace std::chrono;
        return Timestamp(system_clock::now());
    }

    friend bool operator<(const Timestamp& lhs, const Timestamp& rhs);
    friend bool operator==(const Timestamp& lhs, const Timestamp& rhs);
    friend bool operator<=(const Timestamp& lhs, const Timestamp& rhs);

    friend Timestamp operator+(const Timestamp& lhs, double sec);

    void swap(Timestamp& other)
    {
        std::swap(_timePoint, other._timePoint);
    }

    static const int microSecsPerSecond = 1000000;

private:
    TimePoint _timePoint;
};

#endif //HYDROGENWEB_TIMESTAMP_HPP
