/*
 * File: HttpResponse.cpp
 * ----------------------
 * @author: Fu Wei
 *
 */

#include "network/http/HttpResponse.hpp"
#include "core/HyBuffer.hpp"

void HttpResponse::appendToBuffer(HyBuffer* outputBuffer) const
{
    char buf[32];
    snprintf(buf, sizeof buf, "HTTP/1.1 %d ", _statusCode);
    outputBuffer->append(buf);
    outputBuffer->append(_statusMsg);
    outputBuffer->append("\r\n");

    if (_closeConnection)
    {
        outputBuffer->append("Connection: close\r\n");
    }
    else
    {
        snprintf(buf, sizeof buf, "Content-Length: %zd\r\n", _body.size());
        outputBuffer->append(buf);
        outputBuffer->append("Connection: keep-alive\r\n");
    }

    for (const auto& header : _headers)
    {
        outputBuffer->append(header.first);
        outputBuffer->append(": ");
        outputBuffer->append(header.second);
        outputBuffer->append("\r\n");
    }

    outputBuffer->append("\r\n");
    outputBuffer->append(_body);
}
