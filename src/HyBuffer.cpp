/*
 * File: HyBuffer.cpp
 * ------------------
 * @author: Fu Wei
 * Implementation of HyBuffer.
 */

#include "core/HyBuffer.hpp"
#include "system/unixUtility.hpp"

#include <sys/uio.h>

void HyBuffer::reserve(size_t length)
{
    if (writableBytes() < length)
    {
        if (writableBytes() + prependableBytes() < length + PREPENDABLEBYTES)
        {
            /** we need to resize */
            _buffer.resize(_writerIndex + length);
        }
        else
        {
            /** move data to the front */
            size_t readable = readableBytes();
            std::copy(_buffer.begin() + _readerIndex,
                      _buffer.begin() + _writerIndex,
                      _buffer.begin() + PREPENDABLEBYTES);
            _readerIndex = PREPENDABLEBYTES;
            _writerIndex = PREPENDABLEBYTES + readable;
        }
    }
}
ssize_t HyBuffer::readFd(int fd)
{
    char tempBuffer[65536];
    const size_t writable = writableBytes();

    struct iovec vec[2];
    vec[0].iov_base = &*_buffer.begin() + _writerIndex;
    vec[0].iov_len = writable;
    vec[1].iov_base = tempBuffer;
    vec[1].iov_len = sizeof tempBuffer;

    const ssize_t n = readv(fd, vec, 2);
    if (n < 0)
    {
        unix_error("HyBuffer::readFd - readv error");
    }
    else if (static_cast<size_t>(n) <= writable)
    {
        _writerIndex += n;
    }
    else
    {
        _writerIndex = _buffer.size();
        append(tempBuffer, static_cast<size_t>(n) - writable);
    }

    return n;
}
