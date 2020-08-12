/*
 * File: HttpContext.cpp
 * ---------------------------------
 * @author: Fu Wei
 *
 */

#include "network/http/HttpContext.hpp"
#include "core/HyBuffer.hpp"

bool HttpContext::parseRequest(HyBuffer* buffer, Timestamp receiveTime)
{
    bool correct = true;
    bool hasMore = true;
    while (hasMore)
    {
        if (_state == EXPECTREQUESTLINE)
        {
            const char* crlf = buffer->getCRLF();
            if (crlf)
            {
                correct = parseRequestLine(buffer->payload(), crlf);
                if (correct) /** request line is ok */
                {
                    _request.setReceiveTime(receiveTime);
                    buffer->clearUntil(crlf + 2);
                    _state = EXPECTHEADERS;
                }
                else
                {
                    hasMore = false;
                }
            }
            else
            {
                hasMore = false;
            }
        }
        else if (_state == EXPECTHEADERS)
        {
            const char* crlf = buffer->getCRLF();
            if (crlf)
            {
                const char* colon = std::find(buffer->payload(), crlf, ':');
                if (colon != crlf)
                {
                    _request.addHeader(buffer->payload(), colon, crlf);
                }
                else
                {
                    // empty line, end of the header fields
                    if (_request.method() == HttpRequest::GET)
                    {
                        hasMore = false;
                        _state = ALLOK;
                    }
                    else
                    {
                        _state = EXPECTBODY;
                    }
                }
                buffer->clearUntil(crlf + 2);
            }
            else
            {
                hasMore = false;
            }
        }
        else if (_state == EXPECTBODY)
        {
            /** check content length */
            int contentLength = std::stoi(_request.getHeader("Content-Length"));
            int actualLength = std::distance(buffer->payload(), buffer->getEOL());
            if (contentLength != actualLength)
            {
                correct = false;
                hasMore = false;
            }

            while (hasMore)
            {
                const char* assignment = buffer->getAssignment();
                if (assignment)
                {
                    const char* andOp = buffer->getAND();
                    if (andOp)
                    {
                        _request.addBody(buffer->payload(), assignment, andOp);
                        buffer->clearUntil(andOp + 1);
                    }
                    else
                    {
                        _request.addBody(buffer->payload(), assignment, buffer->getEOL());
                        buffer->clearUntil(buffer->getEOL() + 1);
                        hasMore = false;
                        _state = ALLOK;
                    }
                }
                else
                {
                    correct = false;
                    hasMore = false;
                }
            }
        }
    }
    return correct;
}

bool HttpContext::parseRequestLine(const char* begin, const char* end)
{
    bool correct = false;
    const char* space = std::find(begin, end, ' ');
    if (space != end && _request.setMethod(begin, space))
    {
        begin = space + 1;
        space = std::find(begin, end, ' ');
        if (space != end)
        {
            /** parse path and query */
            const char* question = std::find(begin, space, '?');
            if (question != space)
            {
                _request.setPath(begin, question);
                _request.setQuery(question + 1, space);
            }
            else
            {
                _request.setPath(begin, space);
            }
            begin = space + 1;
            /** parse HTTP version */
            // FIXME: only supports HTTP1.0/1.1
            correct = (end - begin == 8) && std::equal(begin, end - 1, "HTTP/1.");
            if (correct)
            {
                if (*(end - 1) == '1')
                {
                    _request.setVersion(HttpRequest::HTTP11);
                }
                else if (*(end - 1) == '0')
                {
                    _request.setVersion(HttpRequest::HTTP10);
                }
                else
                {
                    correct = false;
                }
            }
        }
    }
    return correct;
}
