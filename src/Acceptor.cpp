/*
 * File: Acceptor.cpp
 * ---------------------------------
 * @author: Fu Wei
 * Implementation of Acceptor.
 */

#include "network/EventLoop.hpp"
#include "network/socketDetail.hpp"
#include "network/Acceptor.hpp"
#include "network/InetAddr.hpp"
#include "system/unixUtility.hpp"

Acceptor::Acceptor(EventLoop* loop, const InetAddr& listenAddr) :
    _loop(loop),
    _listenSocket(socketDetail::nonblockingSocket()),
    _acceptorChannel(loop, _listenSocket.fd()),
    _listening(false)
{
    // Eliminates "Address already in use" error from bind
    _listenSocket.setReuse(true);

    // bind the server socket address to the sockfd of _listenSocket
    _listenSocket.bindAddr(listenAddr);
    _acceptorChannel.setReadCallback(std::bind(&Acceptor::incomingHandler, this));
    /* Acceptor is ready for listening now */
}

void Acceptor::listen()
{
    _loop->assertInLoopThread();
    _listening = true;
    _listenSocket.listen();
    _acceptorChannel.enableReading();
    /* Acceptor is ready for poll now */
}

void Acceptor::incomingHandler()
{
    _loop->assertInLoopThread();

    InetAddr newConnectionAddr(0);
    int connfd = _listenSocket.accept(newConnectionAddr);
    // TODO: if no more fd
    if (connfd >= 0)
    {
        if (_newConnectionCallback)
        {
            // move the Socket object to make sure the connfd will be eventually closed
            Socket incoming(connfd);
            _newConnectionCallback(std::move(incoming), newConnectionAddr);
        }
        else
        {
            ::Close(connfd); // callback not set, just close the connfd
        }
    }
}
