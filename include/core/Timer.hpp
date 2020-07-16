/*
 * File: Timer.hpp
 * ---------------
 * @author: Fu Wei
 * Timer is a basic component of timer classes family. Each Timer has its own call back function
 * and expiration time. User should not use Timer directly but TimerQueue instead. TimerQueue
 * do the management job of Timers while EventLoop adds them to a TimerQueue.
 * // TODO: repeat feature
 */

#ifndef HYDROGENWEB_TIMER_HPP
#define HYDROGENWEB_TIMER_HPP

#include "core/TimeStamp.hpp"

#include <functional>

typedef std::function<void()> TimerCallBack;

class Timer
{
public:
    Timer(const TimerCallBack& cb, TimeStamp endTime, double interval) :
        _cb(cb), _endTime(endTime), _interval(interval)
    {
    }

    void run() const { _cb(); };

    TimeStamp expiredTime() const { return _endTime; }

    /*
     * make it non-copyable
     */
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

private:
    TimerCallBack _cb;
    TimeStamp _endTime; // expiration time
    double _interval;
};
#endif //HYDROGENWEB_TIMER_HPP
