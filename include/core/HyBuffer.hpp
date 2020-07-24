/*
 * File: HyBuffer.hpp
 * ------------------
 * @author: Fu Wei
 * Interface of HyBuffer - a read/write buffer class. Inspired by the buffer design of
 * libevent/netty/muduo.
 */

#ifndef HYDROGENWEB_HYBUFFER_HPP
#define HYDROGENWEB_HYBUFFER_HPP

#include <vector>
#include <string>
#include <cassert>

class HyBuffer
{
public:
    static const size_t PREPENDABLEBYTES = 8; // 8 bytes
    static const size_t INITIALSIZE = 1024; // 1KB

    HyBuffer() :
        _buffer(PREPENDABLEBYTES + INITIALSIZE),
        _readerIndex(PREPENDABLEBYTES),
        _writerIndex(PREPENDABLEBYTES)
    {
    }

    size_t prependableBytes() const { return _readerIndex; }
    size_t readableBytes() const { return _writerIndex - _readerIndex; }
    size_t writableBytes() const { return _buffer.size() - _readerIndex; }

    void clear(size_t length) { _readerIndex += length; }
    void clearAll()
    {
        _readerIndex = PREPENDABLEBYTES;
        _writerIndex = PREPENDABLEBYTES;
    }

    const char* payload() const { return _buffer.data() + _readerIndex; }

    std::string readAsString()
    {
        std::string contentStr(payload(),
                               payload() + readableBytes());
        clearAll();
        return contentStr;
    }

    void append(const char* res, size_t length)
    {
        reserve(length);
        std::copy(res, res + length, _buffer.begin() + _writerIndex);
        _writerIndex += length;
    }

    void append(const void* res, size_t length)
    {
        append(static_cast<const char*>(res), length);
    }

    void append(const std::string& res)
    {
        append(res.data(), res.length());
    }

    void prepend(const void* res, size_t length)
    {
        assert(length <= prependableBytes());

        _readerIndex -= length;
        auto data = static_cast<const char*>(res);
        std::copy(data, data + length, _buffer.begin() + _readerIndex);
    }

    ssize_t readFd(int fd);

private:
    void reserve(size_t length);

    std::vector<char> _buffer;
    size_t _readerIndex;
    size_t _writerIndex;
};

#endif //HYDROGENWEB_HYBUFFER_HPP
