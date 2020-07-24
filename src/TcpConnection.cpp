/*
 * File: TcpConnection.cpp
 * -----------------------
 * @author: Fu Wei
 * Implementation of TcpConnection.
 */

#include "network/TcpConnection.hpp"
#include "network/Channel.hpp"
#include "network/EventLoop.hpp"
#include "system/unixUtility.hpp"

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

    _channel->setReadCallback(std::bind(&TcpConnection::connectionHandler, this, std::placeholders::_1));
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
    assert(_state == CONNECTED || _state == DISCONNECTING);

    setState(DISCONNECTED);
    _channel->disableAll(); // a TcpConnection may be destroyed directly without closeHandler()
    _connCallback(shared_from_this());
    _loop->removeChannel(_channel.get());
}

void TcpConnection::connectionHandler(TimeStamp receiveTime)
{
    size_t n = _inputBuffer.readFd(_socket.fd());
    if (n > 0)
    {
        if (_msgCallback)
        {
            _msgCallback(shared_from_this(), _inputBuffer, receiveTime);
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
    if (_channel->isWriting())
    {
        ssize_t n = ::write(_socket.fd(),
                            _outputBuffer.payload(),
                            _outputBuffer.readableBytes());
        if (n > 0) /** write some */
        {
            _outputBuffer.clear(static_cast<size_t>(n));
            if (_outputBuffer.readableBytes() == 0) /** write completely */
            {
                _channel->disableWriting();
                if (_state == DISCONNECTING)
                {
                    shutdownInLoopThread();
                }
            }
            else /** not completely */
            {
                spdlog::info("TcpConnection::writeHandler- wrote {} bytes (not complete)", n);
            }
        }
        else /** write error */
        {
            unix_error("TcpConnection::writeHandler - write error");
        }
    }
    else /** the connection is already down */
    {
        spdlog::info("TcpConnection::writeHandler - Connection is disconnected, cannot write");
    }
}

// is bind to TcpServer's removeConnection()
void TcpConnection::closeHandler()
{
    _loop->assertInLoopThread();

    spdlog::info("TcpConnection::closeHandler() - state = {}", _state);

    assert(_state == CONNECTED || _state == DISCONNECTING);

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

void TcpConnection::send(const std::string& msg)
{
    if (_state == CONNECTED)
    {
        if (_loop->isInLoopThread())
        {
            sendInLoopThread(msg);
        }
        else
        {
            // TODO: avoid copying the msg
            _loop->runInLoopThread(std::bind(&TcpConnection::sendInLoopThread,
                                             this,
                                             msg));
        }
    }
}

void TcpConnection::sendInLoopThread(const std::string& msg)
{
    _loop->assertInLoopThread();

    ssize_t n = 0;
    if (!_channel->isWriting() && _outputBuffer.readableBytes() == 0)
    { /** try writing data directly */
        n = ::write(_socket.fd(), msg.data(), msg.length());
        if (n >= 0)
        {
            if (static_cast<size_t>(n) < msg.length())
            {
                spdlog::info("TcpConnection::sendInLoopThread - wrote {}/{} bytes",
                             n,
                             msg.length());
            }
        }
        else if (n < 0)
        {
            n = 0;
            unix_error("TcpConnection::sendInLoopThread - write error");
        }
    }

    if (static_cast<size_t>(n) < msg.length())
    { /** append remaining data to buffer and indicate write event for poller */
        _outputBuffer.append(msg.data() + n, msg.length() - n);
        if (!_channel->isWriting())
        {
            _channel->enableWriting();
        }
    }
}

void TcpConnection::shutdown()
{
    if (_state == CONNECTED)
    {
        //TODO: compare and swap
        setState(DISCONNECTING);
        /** use shared_from_this() to retain this TcpConnection */
        _loop->runInLoopThread(std::bind(&TcpConnection::shutdownInLoopThread,
                                         shared_from_this()));
    }
}

void TcpConnection::shutdownInLoopThread()
{
    _loop->assertInLoopThread();

    if (!_channel->isWriting())
    {
        _socket.shutdownWrite(); /** no more transmissions */
    }
}
