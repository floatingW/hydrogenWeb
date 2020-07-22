/*
 * File: Channel.cpp
 * ---------------------------------
 * @author: Fu Wei
 *
 */

#include "network/Channel.hpp"
#include "network/EventLoop.hpp"

#include <poll.h>

#include "spdlog/spdlog.h"

const int Channel::EMPTYEVENT = 0;
const int Channel::READEVENT = POLLIN | POLLPRI;
const int Channel::WRITEEVENT = POLLOUT;

Channel::Channel(EventLoop* loop, int fd) :
    _loop(loop), _fd(fd), _events(0), _revents(0), _index(-1)
{
}

Channel::~Channel()
{
    assert(!_handlingEvent); /* should definitely not handleEvent now */
}

void Channel::handleEvent(TimeStamp receiveTime)
{
    _handlingEvent = true;

    if (_revents & POLLNVAL)
    {
        spdlog::warn("Channel::handleEvent() - Invalid polling request");
    }
    if ((_revents & POLLHUP) && !(_revents & POLLIN))
    {
        spdlog::warn("Channel::handleEvent() - hung up");
        if (_closeCallback)
        {
            _closeCallback();
        }
    }
    if (_revents & (POLLERR | POLLNVAL))
    {
        if (_errorCallback)
            _errorCallback();
    }
    if (_revents & (POLLIN | POLLPRI | POLLRDHUP))
    {
        if (_readCallback)
            _readCallback(receiveTime);
    }
    if (_revents & POLLOUT)
    {
        if (_writeCallback)
            _writeCallback();
    }

    _handlingEvent = false;
}

void Channel::update()
{
    _loop->updateChannel(this);
}
