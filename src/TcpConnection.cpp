/*
 * File: TcpConnection.cpp
 * -----------------------
 * @author: Fu Wei
 * Implementation of TcpConnection.
 */

#include "network/TcpConnection.hpp"
#include "network/Channel.hpp"
#include "network/EventLoop.hpp"

#include <unistd.h> // for read(3)

#include "spdlog/spdlog.h"

TcpConnection::TcpConnection(EventLoop* loop,
                             std::string& connName,
                             Socket socket,
                             const InetAddr& serverAddr,
                             const InetAddr& clientAddr) :
    _loop(loop),
    _connName(connName),
    _state(CONNECTING),
    _socket(std::move(socket)),
    _channel(new Channel(loop, _socket.fd())),
    _serverAddr(serverAddr),
    _clientAddr(clientAddr)
{
    spdlog::info("new TcpConnection {} at {} fd = {}",
                 _connName,
                 (void*)this,
                 _socket.fd());

    _channel->setReadCallback(std::bind(&TcpConnection::connectionHandler, this));
    _channel->setWriteCallback(std::bind(&TcpConnection::writeHandler, this));
    _channel->setCloseCallback(std::bind(&TcpConnection::closeHandler, this));
    _channel->setErrorCallback(std::bind(&TcpConnection::errorHandler, this));
}

TcpConnection::~TcpConnection()
{
    spdlog::info("Tcp Connection {} at {} fd = {} destructed",
                 _connName,
                 (void*)this,
                 _channel->fd());
}

void TcpConnection::establishConnection()
{
    _loop->assertInLoopThread();
    assert(_state == CONNECTING);

    setState(CONNECTED);
    _channel->enableReading();
    _connCallback(shared_from_this());
}

void TcpConnection::destroyConnection()
{
    _loop->assertInLoopThread();
    assert(_state == CONNECTED);

    setState(DISCONNECTED);
    _channel->disableAll(); // a TcpConnection may be destroyed directly without closeHandler()
    _connCallback(shared_from_this());
    _loop->removeChannel(_channel.get());
}

void TcpConnection::connectionHandler()
{
    char buf[65536];
    ssize_t n = ::read(_channel->fd(), buf, sizeof buf);
    if (n > 0)
    {
        if (_msgCallback)
        {
            _msgCallback(shared_from_this(), buf, n);
        }
    }
    else if (n == 0)
    {
        closeHandler();
    }
    else
    {
        errorHandler();
    }
}

void TcpConnection::writeHandler()
{
}

// is bind to TcpServer's removeConnection()
void TcpConnection::closeHandler()
{
    _loop->assertInLoopThread();

    spdlog::info("TcpConnection::closeHandler() - state = {}", _state);

    assert(_state == CONNECTED);

    _channel->disableAll();

    assert(_closeCallback);

    _closeCallback(shared_from_this()); // send this TcpConnection to TcpServer
}

void TcpConnection::errorHandler()
{
    // don't close connection here, just logging
    // connection will be closed normally
    // TODO: more detailed error msg about sockfd
    spdlog::error("TcpConnection::errorHandler() - [{}]", _connName);
}
