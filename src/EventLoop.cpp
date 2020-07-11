/*
 * File: EventLoop.cpp
 * ---------------------------------
 * @author: Fu Wei
 *
 */

#include "network/EventLoop.hpp"
#include "network/Poller.hpp"
#include "network/Channel.hpp"

#include <cassert>

#include <poll.h>

#include "spdlog/spdlog.h"

const int POLLTIME = 10000; // millisecond

thread_local EventLoop* loopInThisThread = nullptr;

EventLoop::EventLoop() :
    _looping(false), _threadId(gettid()), _quit(false), _poller(new Poller(this))
{
    spdlog::info("A new eventLoop {} created in thread {}", (void*)this, _threadId);
    if (loopInThisThread)
    {
        spdlog::critical("an eventLoop {} exists in this thread {}",
                         (void*)loopInThisThread,
                         _threadId);
    }
    else
    {
        loopInThisThread = this;
    }
}

EventLoop::~EventLoop()
{
    assert(!_looping);

    loopInThisThread = nullptr;
}

void EventLoop::loop()
{
    assert(!_looping);

    assertInLoopThread();
    _looping = true;
    _quit = false;

    while (!_quit)
    {
        _activeChannels.clear();
        _poller->poll(POLLTIME, &_activeChannels);
        for (auto ich = _activeChannels.cbegin(); ich < _activeChannels.cend(); ++ich)
        {
            (*ich)->handleEvent();
        }
    }

    spdlog::info("EventLoop {} stop looping", (void*)this);
    _looping = false;
}

void EventLoop::quit()
{
    _quit = true;
    // TODO: wakeup loop thread
}

void EventLoop::abortNotInLoopThread()
{
    spdlog::critical("EventLoop {} was created in thread {}, current thread is {}",
                     (void*)this,
                     _threadId,
                     gettid());
    _exit(-1);
}

void EventLoop::updateChannel(Channel* channel)
{
    assert(channel->ownerLoop() == _loop);

    assertInLoopThread();
    _poller->updateChannel(channel);
}

EventLoop* EventLoop::getCurrentEventLoop()
{
    return loopInThisThread;
}
