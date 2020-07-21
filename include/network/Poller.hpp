/*
 * File: Poller.hpp
 * ---------------------------------
 * @author: Fu Wei
 *
 */

#ifndef HYDROGENWEB_POLLER_HPP
#define HYDROGENWEB_POLLER_HPP

#include "network/EventLoop.hpp"
#include "core/TimeStamp.hpp"

#include <vector>
#include <map>

#include <poll.h>

// forward declaration
class Channel;

class Poller
{
public:
    typedef std::vector<Channel*> ChannelList;

    explicit Poller(EventLoop* loop);
    ~Poller();

    TimeStamp poll(int timeoutMs, ChannelList* activeChannels);

    // Called in its loop thread
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    /*
     * precondition checker
     */
    void assertInLoopThread()
    {
        _loop->assertInLoopThread();
    }

    /*
     * make it non-copyable
     */
    Poller(const Poller&) = delete;
    Poller& operator=(const Poller&) = delete;

private:
    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

    typedef std::vector<struct pollfd> fdList;
    typedef std::map<int, Channel*> ChannelMap;

    EventLoop* _loop;
    fdList _fds;
    ChannelMap _channels;
};

#endif //HYDROGENWEB_POLLER_HPP
