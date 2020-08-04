/*
 * File: EventLoopThreadPool.hpp
 * ---------------------------------
 * @author: Fu Wei
 *
 */

#ifndef HYDROGENWEB_EVENTLOOPTHREADPOOL_HPP
#define HYDROGENWEB_EVENTLOOPTHREADPOOL_HPP

#include <vector>

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool
{
public:
    explicit EventLoopThreadPool(EventLoop* loop, size_t numThreads = 0);
    void run();
    EventLoop* nextLoop();

private:
    EventLoop* _mainLoop;
    size_t _numThreads;
    size_t _nextIndex;
    bool _started;
    std::vector<EventLoop*> _loops;
    std::vector<EventLoopThread*> _threads;
};

#endif //HYDROGENWEB_EVENTLOOPTHREADPOOL_HPP
