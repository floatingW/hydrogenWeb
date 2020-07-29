/*
 * File: HttpServer.hpp
 * --------------------
 * @author: Fu Wei
 * Interface of HttpServer. An Encapsulation of TcpServer. Providing convenient interfaces
 * for building a HTTP server on a specific port.
 */

#ifndef HYDROGENWEB_HTTPSERVER_HPP
#define HYDROGENWEB_HTTPSERVER_HPP

#include "network/TcpServer.hpp"

using namespace std::placeholders;

class HttpRequest;
class HttpResponse;

class HttpServer
{
public:
    typedef std::function<void(const HttpRequest&, HttpResponse*)> HttpCallback;

    HttpServer(EventLoop* loop, const InetAddr& addr) :
        _tcpServer(loop, addr)
    {
        _tcpServer.setConnectionCallback(std::bind(&HttpServer::onConnection, this, _1));
        _tcpServer.setMessageCallback(std::bind(&HttpServer::onMessage, this, _1, _2, _3));
    }

    void setHttpCallback(const HttpCallback& cb)
    {
        _httpCallback = cb;
    }

    void run();

private:
    void onConnection(const pTcpConnection& conn);
    void onMessage(const pTcpConnection& conn, HyBuffer* buffer, Timestamp receiveTime);
    void processRequest(const pTcpConnection& conn, const HttpRequest& request);

    TcpServer _tcpServer;
    HttpCallback _httpCallback;
};

#endif //HYDROGENWEB_HTTPSERVER_HPP
