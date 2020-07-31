/*
 * File: Acceptor.hpp
 * ---------------------------------
 * @author: Fu Wei
 * Interface of Acceptor, an internal class that is used to accept new connections and
 * provide callback interface for the higher-level components.
 */

#ifndef HYDROGENWEB_ACCEPTOR_HPP
#define HYDROGENWEB_ACCEPTOR_HPP

#include "network/Socket.hpp"
#include "network/Channel.hpp"

#include <functional>

class EventLoop;
class InetAddr;

class Acceptor
{
    typedef std::function<void(Socket&&, const InetAddr&)> NewConnectionCallback;

public:
    Acceptor(EventLoop* loop, const InetAddr& addr);

    void setNewConnectionCallback(const NewConnectionCallback& cb) { _newConnectionCallback = cb; }

    void listen();
    bool listening() const { return _listening; }

    /*
     * non-copyable class
     */
    Acceptor(const Acceptor&) = delete;
    Acceptor& operator=(const Acceptor&) = delete;

private:
    void incomingHandler();

    EventLoop* _loop;
    NewConnectionCallback _newConnectionCallback;
    bool _listening;
    Socket _listenSocket;
    Channel _acceptorChannel;
};

#endif //HYDROGENWEB_ACCEPTOR_HPP
