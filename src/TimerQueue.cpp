/*
 * File: TimerQueue.cpp
 * ---------------------------------
 * @author: Fu Wei
 * Implementation of TimerQueue
 */

#include "network/EventLoop.hpp"
#include "core/TimerQueue.hpp"
#include "system/unixUtility.hpp"

#include <cassert> // for assert macro
#include <cstring> // memset()

#include <sys/timerfd.h> // timerfd_settime(), struct itimerspec

TimerQueue::TimerQueue(EventLoop* loop) :
    _loop(loop), _timerfd(Timerfd_create()), _timerChannel(loop, _timerfd), _timers()
{
    // Register the channel of TimerQueue to its own EventLoop
    _timerChannel.setReadCallBack([this] { timerHandler(); });
    _timerChannel.enableReading();
}

TimerQueue::~TimerQueue()
{
    ::close(_timerfd);
}

void TimerQueue::addTimer(const TimerQueue::TimerCallBack& cb, TimeStamp endTime, double interval)
{
    _loop->assertInLoopThread();

    auto pTimer = new Timer(cb, endTime, interval);
    bool first = insert(pTimer);

    if (first) // reset timerfd of this TimerQueue with the earliest expiration time
    {
        resetTimerfd(endTime);
    }
}

void TimerQueue::timerHandler()
{
    _loop->assertInLoopThread();

    auto now = TimeStamp::now();

    // must read 8bytes from timerfd if poller is LT mode
    readFd();

    auto expiration = getExpiration(now);

    for (auto& i : expiration)
    {
        i.second->run();
    }

    reset(std::move(expiration), now);
}

std::vector<TimerQueue::TimerEntry> TimerQueue::getExpiration(TimeStamp now)
{
    std::vector<TimerEntry> expiration;

    while (!_timers.empty() && _timers.cbegin()->first <= now)
    {
        expiration.emplace_back(std::move(_timers.extract(_timers.begin()).value()));
    }

    return std::move(expiration);
}

/*
 * TimerQueue::reset - each time while finishing expired events, must reset timerfd with the
 * earliest Timer in the remaining Timers and re-insert the repeat timers to TimerQueue
 */
void TimerQueue::reset(std::vector<TimerEntry> expired, TimeStamp now)
{
    auto expiredSize = expired.size();
    for (auto i = 0; i < expiredSize; i++)
    {
        auto& entry = expired[i];
        if (entry.second->repeat())
        {
            entry.second->restart(now);
            insert(entry.second.release());
        }
    }

    if (!_timers.empty())
    {
        TimeStamp newExpiration = _timers.cbegin()->second->expiredTime();
        resetTimerfd(newExpiration);
    }
}

bool TimerQueue::insert(Timer* pTimer)
{
    bool first = false;
    TimeStamp expiration = pTimer->expiredTime();
    auto it = _timers.cbegin();

    if (it == _timers.cend() || expiration < it->first)
    {
        first = true;
    }
    std::unique_ptr<Timer> uniquePtr(pTimer);
    auto ret = _timers.insert(std::make_pair(expiration, std::move(uniquePtr)));

    assert(ret.second);

    return first;
}

int TimerQueue::resetTimerfd(TimeStamp expiration) const
{
    struct itimerspec new_value;
    struct itimerspec old_value;
    ::memset(&new_value, 0, sizeof new_value);
    ::memset(&old_value, 0, sizeof old_value);

    // get duration from now, and set itimerspec
    int64_t microSecs = expiration.toMicroSec() - TimeStamp::now().toMicroSec();

    assert(microSecs > 0);

    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microSecs / TimeStamp::microSecsPerSecond);
    ts.tv_nsec = static_cast<long>((microSecs % TimeStamp::microSecsPerSecond) * 1000);
    new_value.it_value = ts;

    // reset timerfd
    int ret = timerfd_settime(_timerfd, 0, &new_value, &old_value);
    if (ret) // timerfd_settime error
    {
        unix_error("timerfd_settime error");
    }

    return ret;
}

void TimerQueue::readFd() const
{
    uint64_t byte;
    ssize_t n = ::read(_timerfd, &byte, sizeof byte);
    if (n != sizeof byte)
    {
        unix_error("read timerfd error");
    }
}
