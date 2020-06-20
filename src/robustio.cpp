/*
 * File: robustio.cpp
 * ------------------
 * @author: Fu Wei
 * The implementation of RIO which is a robust I/O functions package coming from CMU's
 * ICS course and CSAPP. It's originally a c project, just adapting it to c++ style.
 *
 * RIO is a set of wrappers that provide efficient and robust I/O in applications, such
 * as network programs that are subject to short counts. It provides two different kinds
 * of functions:
 * - Unbuffered input and output of binary data: readn, writen
 * - Buffered input of text lines and binary data: readlineb, readnb
 */

#include "io/robustio.hpp"

/*
 * Rio buffered version
 */
Riob::Riob(int fd)
{
    _fd = fd;
    _cnt = 0;
    _bufptr = _buf;
}

/*
 * readlineb - read a line with maxlen (buffered)
 */
ssize_t Riob::readlineb(void* usrbuf, size_t maxlen)
{
    int n;
    int rc;
    char c;
    char* bufp = (char*)usrbuf;

    for (n = 1; n < maxlen; n++)
    {
        if ((rc = _read(&c, 1)) == 1)
        {
            *bufp++ = c;
            if (c == '\n')
            {
                break;
            }
        }
        else if (rc == 0)
        {
            if (n == 1) // EOF, no data read
            {
                return 0;
            }
            else // EOF, some data was read
            {
                break;
            }
        }
        else
        {
            return -1; // error happened
        }
    }
    *bufp = 0;
    return n;
}

/*
 * readnb - read n bytes (buffered)
 */
ssize_t Riob::readnb(void* usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nread;
    char* bufp = (char*)usrbuf;

    while (nleft > 0)
    {
        if ((nread = _read(bufp, nleft)) < 0)
        {
            if (errno == EINTR)
            { // if interrupted by sig handler return, call read() again
                nread = 0;
            }
            else
            {
                return -1; //errno set by read()
            }
        }
        else if (nread == 0)
        { // EOF
            break;
        }
        nleft -= nread;
        bufp += nread;
    }
    return (n - nleft);
}
/*
 * _read - a wrapper for Unix read() function. Use the internal buffer to reduce the number
 * of system call read()
 */
ssize_t Riob::_read(char* usrbuf, size_t n)
{
    // refill if buffer is empty
    while (_cnt <= 0)
    {
        _cnt = read(_fd, _buf, sizeof(_buf));
        if (_cnt < 0)
        {
            if (errno != EINTR) // interrupted by sig handler return
            {
                return -1;
            }
        }
        else if (_cnt == 0) // EOF
        {
            return 0;
        }
        else
        {
            _bufptr = _buf; // reset the buffer pointer
        }
    }

    // copy min(n, _cnt) bytes from internal buffer to usrbuf
    int cnt = n;
    if (_cnt < n)
    {
        cnt = _cnt;
    }
    memcpy(usrbuf, _bufptr, cnt);
    _bufptr += cnt;
    _cnt -= cnt;
    return cnt;
}

/*
 * readn - read n bytes (unbuffered)
 */
ssize_t Rio::readn(int fd, void* usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nread;
    char* bufp = (char*)usrbuf;

    while (nleft > 0)
    {
        if ((nread = read(fd, bufp, nleft)) < 0)
        {
            if (errno == EINTR) // if interrupted by sig handler return, call read() again
            {
                nread = 0;
            }
            else
            {
                return -1;
            }
        }
        else if (nread == 0)
        {
            break;
        }
        nleft -= nread;
        bufp += nread;
    }
    return (n - nleft);
}

/*
 * writen - write n bytes (unbuffered)
 */
ssize_t Rio::writen(int fd, void* usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nwritten;
    char* bufp = (char*)usrbuf;

    while (nleft > 0)
    {
        if ((nwritten = write(fd, bufp, nleft)) <= 0)
        {
            if (errno == EINTR) // if interrupted by sig handler return, call write() again
            {
                nwritten = 0;
            }
            else
            {
                return -1;
            }
        }
        nleft -= nwritten;
        bufp += nwritten;
    }
    return n;
}
