/*
 * File: TcpServer.hpp
 * -------------------
 * @author: Fu Wei
 * Interface of TcpServer, which is a manager of TcpConnection. Providing user interface for accept
 * new TCP connection.
 */

#ifndef HYDROGENWEB_TCPSERVER_HPP
#define HYDROGENWEB_TCPSERVER_HPP

#include "network/TcpConnection.hpp"
#include "network/Acceptor.hpp"

#include <unordered_map>
#include <string>
#include <memory>
#include <functional>

class EventLoop;
class InetAddr;
class TcpConnection;
class Socket;

class TcpServer
{
    typedef std::shared_ptr<TcpConnection> pTcpConnection;
    typedef std::function<void(const pTcpConnection&)> ConnectionCallback;
    typedef std::function<void(const pTcpConnection&,
                               const char* msg,
                               ssize_t length)>
        MessageCallback;

public:
    TcpServer(EventLoop* loop, const InetAddr& listenAddr);

    void run();

    void setConnectionCallback(const ConnectionCallback& cb) { _connCallback = cb; }
    void setMessageCallback(const MessageCallback& cb) { _msgCallback = cb; }

    /*
     * non-copyable
     */
    TcpServer(const TcpServer&) = delete;
    TcpServer& operator=(const TcpServer&) = delete;

private:
    void newConnectionHandler(Socket socket, const InetAddr& clientAddr);
    void removeConnection(const pTcpConnection& conn);

    typedef std::unordered_map<std::string, pTcpConnection> ConnectionMap;

    EventLoop* _loop;
    const std::string _serverName;
    std::unique_ptr<Acceptor> _acceptor;
    ConnectionCallback _connCallback;
    MessageCallback _msgCallback;
    bool _started;
    int _newConnId;
    ConnectionMap _connections;
};

#endif //HYDROGENWEB_TCPSERVER_HPP
