/*
 * File: TcpConnection.hpp
 * -----------------------
 * @author: Fu Wei
 * Interface of TcpConnection. Each TcpConnection is a an independent connection that manages the
 * life cycle of its own Socket member.
 */

#ifndef HYDROGENWEB_TCPCONNECTION_HPP
#define HYDROGENWEB_TCPCONNECTION_HPP

#include "network/InetAddr.hpp"
#include "network/Socket.hpp"
#include "core/GlobalCallbacks.hpp"
#include "core/HyBuffer.hpp"

#include <string>
#include <functional>
#include <memory>

class EventLoop;
class Channel;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop* loop,
                  std::string& connName,
                  Socket socket,
                  const InetAddr& serverAddr,
                  const InetAddr& clientAddr);
    ~TcpConnection();

    const std::string& name() const { return _connName; }
    bool connected() const { return _state == CONNECTED; }
    const InetAddr& serverAddr() { return _serverAddr; }
    const InetAddr& clientAddr() { return _clientAddr; }

    void setConnectionCallback(const ConnectionCallback& cb) { _connCallback = cb; }
    void setMessageCallback(const MessageCallback& cb) { _msgCallback = cb; }
    void setCloseCallback(const CloseCallback& cb) { _closeCallback = cb; }

    void establishConnection();
    void destroyConnection();

    void send(const std::string& msg);
    void shutdown();

    /*
     * non-copyable
     */
    TcpConnection(const TcpConnection&) = delete;
    TcpConnection& operator=(const TcpConnection&) = delete;

private:
    enum State
    {
        CONNECTING,
        CONNECTED,
        DISCONNECTED,
        DISCONNECTING
    };

    void setState(State s) { _state = s; }
    void connectionHandler(Timestamp receiveTime);
    void writeHandler();
    void closeHandler();
    void errorHandler();
    void sendInLoopThread(const std::string& msg);
    void shutdownInLoopThread();

    EventLoop* _loop;
    std::string _connName;
    State _state;
    Socket _socket;
    std::unique_ptr<Channel> _channel;
    InetAddr _serverAddr;
    InetAddr _clientAddr;
    ConnectionCallback _connCallback;
    MessageCallback _msgCallback;
    CloseCallback _closeCallback;
    HyBuffer _inputBuffer;
    HyBuffer _outputBuffer;
};

#endif //HYDROGENWEB_TCPCONNECTION_HPP
