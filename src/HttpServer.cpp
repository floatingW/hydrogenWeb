/*
 * File: HttpServer.cpp
 * --------------------
 * @author: Fu Wei
 *
 */

#include "network/http/HttpServer.hpp"
#include "network/http/HttpContext.hpp"
#include "network/http/HttpResponse.hpp"

#include <spdlog/spdlog.h>

void HttpServer::run()
{
    spdlog::info("HttpServer starts listening");
    _tcpServer.run();
}

void HttpServer::onConnection(const pTcpConnection& conn)
{
    if (conn->connected())
    {
        conn->setContext(HttpContext());
    }
}

void HttpServer::onMessage(const pTcpConnection& conn, HyBuffer* buffer, Timestamp receiveTime)
{
    auto* pContext = std::any_cast<HttpContext>(conn->getContext());

    if (!pContext->parseRequest(buffer, receiveTime))
    {
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }

    if (pContext->allOk())
    {
        processRequest(conn, pContext->request());
        pContext->reset();
    }
}

void HttpServer::processRequest(const pTcpConnection& conn, const HttpRequest& request)
{
    const string& connection = request.getHeader("Connection");
    bool close = connection == "closed" ||
                 (request.version() == HttpRequest::HTTP10 && connection != "keep-alive");
    HttpResponse response(close);
    _httpCallback(request, &response);
    HyBuffer buf;
    response.appendToBuffer(&buf);
    conn->send(&buf);
    if (response.nonPersistent())
    {
        conn->shutdown();
    }
}
