/*
 * File: HttpRequest.hpp
 * ---------------------
 * @author: Fu Wei
 * A wrapper of HTTP request. Storing parsed HTTP request content.
 */

#ifndef HYDROGENWEB_HTTPREQUEST_HPP
#define HYDROGENWEB_HTTPREQUEST_HPP

#include "core/Timestamp.hpp"

#include <cctype> // isspace()
#include <cassert>
#include <string>
#include <map>

using std::string;

class HttpRequest
{
public:
    enum Method
    {
        INVALID,
        GET,
        POST
    };
    enum Version
    {
        UNKNOWN,
        HTTP10,
        HTTP11
    };

    HttpRequest() :
        _method(INVALID), _version(UNKNOWN)
    {
    }

    void setVersion(Version ver)
    {
        assert(_version == UNKNOWN);

        _version = ver;
    }

    Version version() const { return _version; }

    bool setMethod(const char* start, const char* end)
    {
        assert(_method == INVALID);

        string m(start, end);
        if (m == "GET")
        {
            _method = GET;
        }
        else if (m == "POST")
        {
            _method = POST;
        }
        else
        {
            _method = INVALID;
        }
        return _method != INVALID;
    }

    Method method() const { return _method; }

    const char* methodString() const
    {
        const char* result = "UNKNOWN";
        switch (_method)
        {
        case GET:
            result = "GET";
            break;
        case POST:
            result = "POST";
            break;
        default:
            break;
        }
        return result;
    }

    void setPath(const char* start, const char* end) { _path.assign(start, end); }
    const string& path() const { return _path; }

    void setQuery(const char* start, const char* end) { _query.assign(start, end); }
    const string& query() const { return _query; }

    void setReceiveTime(Timestamp t) { _receiveTime = t; }
    Timestamp receiveTime() const { return _receiveTime; }

    void addHeader(const char* begin, const char* colon, const char* end)
    {
        /** A HTTP header string:
            name: value          */

        string name(begin, colon);

        ++colon;
        while (colon < end && ::isspace(*colon))
        {
            ++colon;
        }
        while (colon < end && ::isspace(*end))
        {
            --end;
        }

        string value(colon, end + 1);
        _headers[name] = value;
    }

    string getHeader(const string& name) const
    {
        string data;
        auto it = _headers.find(name);

        if (it != _headers.end())
        {
            data = it->second;
        }

        return data;
    }

    const auto& headers() const
    {
        return _headers;
    }

    void swap(HttpRequest& other)
    {
        std::swap(_method, other._method);
        std::swap(_version, other._version);
        _path.swap(other._path);
        _query.swap(other._query);
        _receiveTime.swap(other._receiveTime);
        _headers.swap(other._headers);
    }

private:
    Method _method;
    Version _version;
    string _path;
    string _query;
    Timestamp _receiveTime;
    std::map<string, string> _headers;
};

#endif //HYDROGENWEB_HTTPREQUEST_HPP
