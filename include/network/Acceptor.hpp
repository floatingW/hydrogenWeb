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
    typedef std::function<void(Socket&&, const InetAddr&)> IncomingCallBack;

public:
    Acceptor(EventLoop* loop, const InetAddr& addr);

    void setIncomingCallBack(const IncomingCallBack& cb) { _incomingCallBack = cb; }

    void listen();
    bool listening() const { return _listening; }

private:
    void incomingHandler();

    IncomingCallBack _incomingCallBack;
    bool _listening;
    EventLoop* _loop;
    Socket _listenSocket;
    Channel _acceptorChannel;
};

#endif //HYDROGENWEB_ACCEPTOR_HPP
