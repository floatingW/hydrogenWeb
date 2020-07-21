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
#include <mutex> // mutex

#include <sys/types.h> // pid_t
#include <unistd.h> // gettid()

// forward declaration
class Channel;
class Poller;
class TimerQueue;
class TimeStamp;

class EventLoop
{
    typedef std::function<void()> TimerCallback;
    typedef std::function<void()> Functor;

public:
    EventLoop();
    ~EventLoop();

    void loop();

    void quit();

    /*
     * set timer
     */
    void runAt(TimeStamp time, const TimerCallback& cb);
    void runAfter(double delay, const TimerCallback& cb);
    void runEvery(double interval, const TimerCallback& cb);

    /*
     * run functions in eventLoop(loop thread)
     */
    void runInLoopThread(const Functor& functor);
    void addToLoopThread(const Functor& functor);

    /*
     * add or reset a channel
     */
    void updateChannel(Channel* channel);
    /*
     * remove a channel
     */
    void removeChannel(Channel* channel);

    void waken() const; // waken the loopthread

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
    void readFd() const; // for wakening
    void processFunctors();

    typedef std::vector<Channel*> ChannelList;

    bool _looping;
    bool _quit;
    bool _processingFunctors;
    const pid_t _threadId;
    std::unique_ptr<Poller> _poller;
    std::unique_ptr<TimerQueue> _timerQueue;
    ChannelList _activeChannels;
    // for waken
    int _wakenfd;
    std::unique_ptr<Channel> _wakenChannel;
    std::mutex _mutex;
    std::vector<Functor> _unprocessedFunctors;
};

#endif //HYDROGENWEB_EVENTLOOP_HPP
