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
    typedef std::function<void(Socket&&, const InetAddr&)> IncomingCallback;

public:
    Acceptor(EventLoop* loop, const InetAddr& addr);

    void setIncomingCallback(const IncomingCallback& cb) { _incomingCallback = cb; }

    void listen();
    bool listening() const { return _listening; }

private:
    void incomingHandler();

    IncomingCallback _incomingCallback;
    bool _listening;
    EventLoop* _loop;
    Socket _listenSocket;
    Channel _acceptorChannel;
};

#endif //HYDROGENWEB_ACCEPTOR_HPP
