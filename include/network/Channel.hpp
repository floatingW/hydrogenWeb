/*
 * File: Channel.hpp
 * ---------------------------------
 * @author: Fu Wei
 *
 */

#ifndef HYDROGENWEB_CHANNEL_HPP
#define HYDROGENWEB_CHANNEL_HPP

#include "core/TimeStamp.hpp"

#include <functional>

// forward declaration
class EventLoop;

class Channel
{
public:
    typedef std::function<void()> EventCallback;
    typedef std::function<void(TimeStamp)> ReadEventCallback;

    Channel(EventLoop* loop, int fd);
    ~Channel();

    void handleEvent(TimeStamp receiveTime);
    void setReadCallback(const ReadEventCallback& cb)
    {
        _readCallback = cb;
    }
    void setWriteCallback(const EventCallback& cb)
    {
        _writeCallback = cb;
    }
    void setErrorCallback(const EventCallback& cb)
    {
        _errorCallback = cb;
    }
    void setCloseCallback(const EventCallback& cb)
    {
        _closeCallback = cb;
    }

    int fd() const { return _fd; }
    int events() const { return _events; }
    void set_revents(int revents) { _revents = revents; }
    bool isEmptyEvent() const { return _events == EMPTYEVENT; }
    bool isWriting() const { return _events & WRITEEVENT; }

    void enableReading()
    {
        _events |= READEVENT;
        update();
    }
    void enableWriting()
    {
        _events |= WRITEEVENT;
        update();
    }
    void disableWriting()
    {
        _events &= ~WRITEEVENT;
        update();
    }
    void disableAll()
    {
        _events = EMPTYEVENT;
        update();
    }

    int index() { return _index; }
    void set_index(int index) { _index = index; }

    EventLoop* ownerLoop() { return _loop; }

    /*
     * make it non-copyable
     */
    Channel(const Channel&) = delete;
    Channel& operator=(const Channel&) = delete;

private:
    void update();

    static const int EMPTYEVENT;
    static const int READEVENT;
    static const int WRITEEVENT;

    EventLoop* _loop;
    const int _fd;
    int _events;
    int _revents;
    int _index; // used by Poller
    bool _handlingEvent;

    ReadEventCallback _readCallback;
    EventCallback _writeCallback;
    EventCallback _errorCallback;
    EventCallback _closeCallback; /* fd closed */
};

#endif //HYDROGENWEB_CHANNEL_HPP
