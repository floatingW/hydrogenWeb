/*
 * File: Poller.cpp
 * ---------------------------------
 * @author: Fu Wei
 *
 */

#include "network/Poller.hpp"
#include "network/Channel.hpp"

#include "spdlog/spdlog.h"

Poller::Poller(EventLoop* loop) :
    _loop(loop)
{
}

Poller::~Poller()
{
}

TimeStamp Poller::poll(int timeoutMs, Poller::ChannelList* activeChannels)
{
    int numEvents = ::poll(&(*_fds.begin()), _fds.size(), timeoutMs);
    TimeStamp now(TimeStamp::now());

    if (numEvents > 0)
    {
        spdlog::info("{} events happened", numEvents);
        fillActiveChannels(numEvents, activeChannels);
    }
    else if (numEvents == 0)
    {
        spdlog::info("nothing happened");
    }
    else
    {
        // error happened
        spdlog::error("Poller::poll() error");
    }

    return now;
}

void Poller::updateChannel(Channel* channel)
{
    assertInLoopThread();
    spdlog::info("fd {} with events {}", channel->fd(), channel->events());

    if (channel->index() < 0) // a new fd
    {
        assert(_channels.find(channel->fd()) == _channels.end());

        // add its pollfd to _fds
        struct pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        _fds.push_back(pfd);

        // add the new channel to _channels and cache its _fds index
        int idx = static_cast<int>(_fds.size()) - 1;
        channel->set_index(idx);
        _channels[pfd.fd] = channel;
    }
    else // existing fd
    {
        assert(_channels.find(channel->fd()) != _channels.end());
        assert(_channels[channel->fd()] == channel);

        int idx = channel->index();
        struct pollfd& pfd = _fds[idx];

        assert(0 <= idx && idx < static_cast<int>(_fds.size()));
        assert(pfd.fd == channel->fd() || pfd.fd == -1);

        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        if (channel->isEmptyEvent())
        {
            // make it negative so the I/O event notification will ignore this channel
            pfd.fd = -(channel->fd()) - 1;
        }
    }
}

void Poller::fillActiveChannels(int numEvents, Poller::ChannelList* activeChannels) const
{
    for (auto ifd = _fds.cbegin();
         ifd != _fds.cend() && numEvents > 0;
         ++ifd)
    {
        if (ifd->revents > 0)
        {
            numEvents--;
            auto ich = _channels.find(ifd->fd);

            assert(ich != _channels.end());

            Channel* channel = ich->second;

            assert(channel->fd() == ifd->fd);

            channel->set_revents(ifd->revents);
            activeChannels->push_back(channel);
        }
    }
}