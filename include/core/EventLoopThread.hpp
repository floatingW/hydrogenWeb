/*
 * File: EventLoopThread.hpp
 * ---------------------------------
 * @author: Fu Wei
 * Interface of EventLoopThread. Provide management of EventLoop.
 */

#ifndef HYDROGENWEB_EVENTLOOPTHREAD_HPP
#define HYDROGENWEB_EVENTLOOPTHREAD_HPP

#include "network/EventLoop.hpp"

#include <mutex>
#include <condition_variable>
#include <thread>

class EventLoopThread
{
public:
    EventLoopThread();
    ~EventLoopThread();

    EventLoop* start();

private:
    void startEventLoop();

    EventLoop* _loop;
    bool _started;
    std::thread _thread;
    std::mutex _mutex;
    std::condition_variable _condVar;
};

#endif //HYDROGENWEB_EVENTLOOPTHREAD_HPP
