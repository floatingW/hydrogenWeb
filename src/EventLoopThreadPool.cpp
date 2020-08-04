/*
 * File: EventLoopThreadPool.cpp
 * ---------------------------------
 * @author: Fu Wei
 *
 */

#include "core/EventLoopThreadPool.hpp"
#include "core/EventLoopThread.hpp"
#include "network/EventLoop.hpp"

#include <cassert>

EventLoopThreadPool::EventLoopThreadPool(EventLoop* loop, size_t numThreads) :
    _mainLoop(loop), _numThreads(numThreads), _nextIndex(0), _started(false)
{
}

void EventLoopThreadPool::run()
{
    assert(!_started);
    _mainLoop->assertInLoopThread();

    _started = true;
    for (size_t i = 0; i < _numThreads; ++i)
    {
        EventLoop* loop = EventLoopThread().start();
        _loops.push_back(loop);
    }
}

EventLoop* EventLoopThreadPool::nextLoop()
{
    _mainLoop->assertInLoopThread();

    EventLoop* nextLoop = _mainLoop;
    if (!_loops.empty())
    {
        nextLoop = _loops[_nextIndex++];
        if (_nextIndex == _loops.size())
        {
            _nextIndex = 0;
        }
    }

    return nextLoop;
}
