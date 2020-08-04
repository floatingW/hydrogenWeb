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

TcpServer::TcpServer(EventLoop* loop, const InetAddr& listenAddr, size_t numThreads) :
    _loop(loop),
    _serverName(listenAddr.toString()),
    _acceptor(new Acceptor(loop, listenAddr)),
    _started(false),
    _newConnId(0),
    _threadPool(new EventLoopThreadPool(loop, numThreads))
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
    EventLoop* itsLoop = _threadPool->nextLoop();
    pTcpConnection pConn = std::make_shared<TcpConnection>(
        itsLoop, connName, std::move(socket), serverAddr, clientAddr);

    _connections[connName] = pConn;
    pConn->setConnectionCallback(_connCallback);
    pConn->setMessageCallback(_msgCallback);
    // FIXME: unsafe
    pConn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, _1));
    itsLoop->addToLoopThread([pConn] { pConn->establishConnection(); });
}

void TcpServer::removeConnection(const pTcpConnection& conn)
{
    _loop->runInLoopThread([this, conn] { removeConnectionInLoopThread(conn); });
}
void TcpServer::removeConnectionInLoopThread(const pTcpConnection& conn)
{
    _loop->assertInLoopThread();

    spdlog::info("TcpServer::removeConnection() - [{}] - {}", _serverName, conn->name());
    size_t n = _connections.erase(conn->name());

    assert(n == 1);

    EventLoop* itsLoop = conn->getLoop();
    itsLoop->addToLoopThread([conn] { conn->destroyConnection(); });
}
