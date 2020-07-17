/*
 * File: TimerQueue.hpp
 * --------------------
 * @author: Fu Wei
 * Interface of TimerQueue, a handler class for Timers. TimerQueue checks for expired events
 * and call the callback function of each Timer.
 */

#ifndef HYDROGENWEB_TIMERQUEUE_HPP
#define HYDROGENWEB_TIMERQUEUE_HPP

#include "core/TimeStamp.hpp"
#include "core/Timer.hpp"
#include "network/Channel.hpp"

#include <set> // set
#include <functional> // function
#include <memory> // unique_ptr

class EventLoop;
class Timer;

class TimerQueue
{
    typedef std::function<void()> TimerCallBack;

public:
    explicit TimerQueue(EventLoop* loop);
    ~TimerQueue();

    void addTimer(const TimerCallBack& cb, TimeStamp endTime, double interval);

private:
    typedef std::pair<TimeStamp, std::unique_ptr<Timer>> TimerEntry;
    typedef std::set<TimerEntry> TimerList;

    void timerHandler();

    std::vector<TimerEntry> getExpiration(TimeStamp now);
    void reset(std::vector<TimerEntry> expired, TimeStamp now);
    bool insert(Timer* pTimer);

    int resetTimerfd(TimeStamp expiration) const;
    void readFd() const;

    void addTimerInLoopThread(Timer* pTimer);

    EventLoop* _loop;
    const int _timerfd;
    Channel _timerChannel;
    TimerList _timers;
};

#endif //HYDROGENWEB_TIMERQUEUE_HPP
