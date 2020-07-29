/*
 * File: HttpResponse.hpp
 * ----------------------
 * @author: Fu Wei
 * Interface of HTTP response class. A wrapper of HTTP response.
 */

#ifndef HYDROGENWEB_HTTPRESPONSE_HPP
#define HYDROGENWEB_HTTPRESPONSE_HPP

#include <string>
#include <map>

class HyBuffer;

using std::string;

class HttpResponse
{
public:
    enum HttpStatusCode
    {
        OK200 = 200,
        BADREQUEST400 = 400,
        NOTFOUND404 = 404,
        UNKNOWN
    };

    explicit HttpResponse(bool close) :
        _statusCode(), _closeConnection(close)
    {
    }

    void setStatusCode(HttpStatusCode code) { _statusCode = code; }
    void setStatusMsg(const string& msg) { _statusMsg = msg; }
    void setCloseConnection(bool close) { _closeConnection = close; }
    bool nonPersistent() const { return _closeConnection; }
    void setContentType(const string& contentType) { addHeader("Content-Type", contentType); }
    void addHeader(const string& key, const string& value) { _headers[key] = value; }
    void setBody(const string& body) { _body = body; }

    void appendToBuffer(HyBuffer* outputBuffer) const;

private:
    std::map<string, string> _headers;
    HttpStatusCode _statusCode;
    string _statusMsg;
    bool _closeConnection;
    string _body;
};

#endif //HYDROGENWEB_HTTPRESPONSE_HPP
