/*
 * File: TcpServer.cpp
 * -------------------
 * @author: Fu Wei
 * Implementation of TcpServer.
 */

#include "network/EventLoop.hpp"
#include "network/TcpServer.hpp"
#include "network/Acceptor.hpp"
#include "network/InetAddr.hpp"
#include "network/Socket.hpp"

#include "spdlog/spdlog.h"

using namespace std::placeholders;

TcpServer::TcpServer(EventLoop* loop, const InetAddr& listenAddr) :
    _loop(loop),
    _serverName(listenAddr.toString()),
    _acceptor(new Acceptor(loop, listenAddr)),
    _started(false),
    _newConnId(0)
{
    _acceptor->setNewConnectionCallback(
        std::bind(&TcpServer::newConnectionHandler, this, _1, _2));
}

void TcpServer::run()
{
    if (!_started)
    {
        _started = true;
    }

    if (!_acceptor->listening())
    {
        _loop->runInLoopThread([pAcceptor = _acceptor.get()] { pAcceptor->listen(); });
    }
}

void TcpServer::newConnectionHandler(Socket socket, const InetAddr& clientAddr)
{
    _loop->assertInLoopThread();

    char buf[32];
    snprintf(buf, sizeof buf, "#%d", _newConnId++);
    std::string connName = _serverName + buf;

    spdlog::info("TcpServer::newConnectionHandler [{}] - new connection [{}] from {}",
                 _serverName,
                 connName,
                 clientAddr.toString());

    InetAddr serverAddr(socketDetail::getLocalAddr(socket.fd()));
    pTcpConnection pConn = std::make_shared<TcpConnection>(
        _loop, connName, std::move(socket), serverAddr, clientAddr);

    _connections[connName] = pConn;
    pConn->setConnectionCallback(_connCallback);
    pConn->setMessageCallback(_msgCallback);
    pConn->establishConnection();
}
