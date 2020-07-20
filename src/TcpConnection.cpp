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

    _channel->setReadCallback([this] { connectionHandler(); });
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

void TcpConnection::connectionHandler()
{
    char buf[65536];
    ssize_t n = ::read(_channel->fd(), buf, sizeof buf);
    _msgCallback(shared_from_this(), buf, n);
    // TODO: close connection while n == 0
}
