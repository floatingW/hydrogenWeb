/*
 * File: EventLoopThread.cpp
 * ---------------------------------
 * @author: Fu Wei
 * Implementation of EventLoopThread.
 */

#include "core/EventLoopThread.hpp"

#include <cassert>

EventLoopThread::EventLoopThread() :
    _started(false), _loop(nullptr)
{
}

EventLoopThread::~EventLoopThread()
{
    if (_thread.joinable())
    {
        _thread.join();
    }
}

EventLoop* EventLoopThread::start()
{
    assert(!_started);

    _thread = std::thread([this] { startEventLoop(); });

    {
        std::unique_lock<std::mutex> lock(_mutex);
        while (!_loop)
        {
            _condVar.wait(lock);
        }
    }

    return _loop;
}

void EventLoopThread::startEventLoop()
{
    EventLoop loop;

    {
        std::scoped_lock<std::mutex> lock(_mutex);
        _loop = &loop;
        _condVar.notify_one();
    }

    loop.loop();
}
