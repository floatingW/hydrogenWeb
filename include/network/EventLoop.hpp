/*
 * File: EventLoop.hpp
 * ---------------------------------
 * @author: Fu Wei
 *
 */

#ifndef HYDROGENWEB_EVENTLOOP_HPP
#define HYDROGENWEB_EVENTLOOP_HPP

#include <vector>
#include <memory> // unique_ptr
#include <functional> // function

#include <sys/types.h> // pid_t
#include <unistd.h> // gettid()

// forward declaration
class Channel;
class Poller;
class TimerQueue;
class TimeStamp;

class EventLoop
{
    typedef std::function<void()> TimerCallBack;

public:
    EventLoop();
    ~EventLoop();

    void loop();

    void quit();

    /*
     * set timer
     */
    void runAt(TimeStamp time, const TimerCallBack& cb);
    void runAfter(double delay, const TimerCallBack& cb);
    void runEvery(double interval, const TimerCallBack& cb);

    /*
     * add or reset a channel
     */
    void updateChannel(Channel* channel);

    /*
     * Get the eventloop of current thread
     */
    static EventLoop* getCurrentEventLoop();

    /*
     * precondition check functions
     */
    void assertInLoopThread()
    {
        if (!isInLoopThread())
        {
            abortNotInLoopThread();
        }
    }

    bool isInLoopThread() const
    {
        return _threadId == gettid();
    }

    /*
     * make it non-copyable
     */
    EventLoop(const EventLoop&) = delete;
    EventLoop& operator=(const EventLoop&) = delete;

private:
    void abortNotInLoopThread();

    typedef std::vector<Channel*> ChannelList;

    bool _looping;
    bool _quit;
    const pid_t _threadId;
    std::unique_ptr<Poller> _poller;
    std::unique_ptr<TimerQueue> _timerQueue;
    ChannelList _activeChannels;
};

#endif //HYDROGENWEB_EVENTLOOP_HPP
