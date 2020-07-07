/*
 * File: TimeStamp.hpp
 * ---------------------------------
 * @author: Fu Wei
 *
 */

#ifndef HYDROGENWEB_TIMESTAMP_HPP
#define HYDROGENWEB_TIMESTAMP_HPP

#include <chrono>

class TimeStamp
{
    typedef std::chrono::system_clock::time_point Time;

public:
    explicit TimeStamp(Time t) :
        _time(t)
    {
    }

    auto toMilliseconds()
    {
        using namespace std::chrono;
        return duration_cast<milliseconds>(_time.time_since_epoch()).count();
    }

    static Time now()
    {
        using namespace std::chrono;
        return system_clock::now();
    }

private:
    Time _time; // timestamp from 19700101000000;
};

#endif //HYDROGENWEB_TIMESTAMP_HPP
