/*
 * File: Channel.hpp
 * ---------------------------------
 * @author: Fu Wei
 *
 */

#ifndef HYDROGENWEB_CHANNEL_HPP
#define HYDROGENWEB_CHANNEL_HPP

#include <functional>

// forward declaration
class EventLoop;

class Channel
{
public:
    typedef std::function<void()> EventCallback;

    Channel(EventLoop* loop, int fd);

    void handleEvent();
    void setReadCallBack(const EventCallback& cb)
    {
        _readCallback = cb;
    }
    void setWriteCallBack(const EventCallback& cb)
    {
        _writeCallback = cb;
    }
    void setErrorCallBack(const EventCallback& cb)
    {
        _errorCallback = cb;
    }

    int fd() const { return _fd; }
    int events() const { return _events; }
    void set_revents(int revents) { _revents = revents; }
    bool isEmptyEvent() const { return _events == EMPTYEVENT; }

    void enableReading()
    {
        _events |= READEVENT;
        update();
    }
    void enableWriting()
    {
        _events |= READEVENT;
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

    EventCallback _readCallback;
    EventCallback _writeCallback;
    EventCallback _errorCallback;
};

#endif //HYDROGENWEB_CHANNEL_HPP
