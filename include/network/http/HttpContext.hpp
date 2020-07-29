/*
 * File: HttpContext.hpp
 * ---------------------
 * @author: Fu Wei
 * Interface of HTTP context class. A HTTP request parser.
 */

#ifndef HYDROGENWEB_HTTPCONTEXT_HPP
#define HYDROGENWEB_HTTPCONTEXT_HPP

#include "network/http/HttpRequest.hpp"

class HyBuffer;

class HttpContext
{
public:
    enum HttpRequestState
    {
        EXPECTREQUESTLINE,
        EXPECTHEADERS,
        EXPECTBODY,
        ALLOK
    };

    HttpContext() :
        _state(EXPECTREQUESTLINE)
    {
    }

    bool parseRequest(HyBuffer* buffer, Timestamp receiveTime);

    bool allOk() const { return _state == ALLOK; }

    void reset()
    {
        _state = EXPECTREQUESTLINE;
        HttpRequest empty;
        _request.swap(empty);
    }

    const HttpRequest& request() const { return _request; }
    HttpRequest& request() { return _request; }

private:
    bool parseRequestLine(const char* begin, const char* end);

    HttpRequestState _state;
    HttpRequest _request;
};

#endif //HYDROGENWEB_HTTPCONTEXT_HPP
