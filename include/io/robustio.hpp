//
// Created by fwei on 6/15/20.
//

#ifndef HYDROGENWEB_ROBUSTIO_H
#define HYDROGENWEB_ROBUSTIO_H

#include <sys/types.h>
#include <cerrno>
#include "cstring"
#include <unistd.h>

const int RIO_BUFSIZE = 8192;

/*
 * buffered Rio
 */
class Riob
{
public:
    // explicit to avoid implicit conversion
    explicit Riob(int fd);

    ssize_t readlineb(void* usrbuf, size_t maxlen);

    ssize_t readnb(void* usrbuf, size_t n);

private:
    int _fd; // descriptor of this internal buf
    int _cnt; // unread bytes in internal buf
    char* _bufptr; // next unread byte in internal buf
    char _buf[RIO_BUFSIZE]; // internal buffer

    ssize_t _read(char* usrbuf, size_t n);
};

/*
 * unbuffered Rio
 */
class Rio
{
public:
    static ssize_t readn(int fd, void* usrbuf, size_t n);

    static ssize_t writen(int fd, void* usrbuf, size_t n);
};

#endif //HYDROGENWEB_ROBUSTIO_H
