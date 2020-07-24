/*
 * File: Socket.cpp
 * ----------------
 * @author: Fu Wei
 * Implementation of Socket class.
 */

#include "network/Socket.hpp"
#include "network/socketDetail.hpp"
#include "system/unixUtility.hpp"
#include "network/InetAddr.hpp"

#include <cstring>
#include <iostream>

Socket::~Socket()
{
    if (_sockfd >= 0)
    { // close the fd if necessary
        ::Close(_sockfd);
    }
}
void Socket::setReuse(bool reuse) const
{
    int optval = reuse ? 1 : 0;
    socketDetail::Setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
}
void Socket::bindAddr(const InetAddr& listenAddr) const
{
    socketDetail::Bind(_sockfd, listenAddr.getSockAddrIN());
}
void Socket::listen() const
{
    socketDetail::Listen(_sockfd, 1024);
}
int Socket::accept(InetAddr& returnAddr)
{
    struct sockaddr_in addr;
    ::memset(&addr, 0, sizeof addr);
    int fd = socketDetail::Accept(_sockfd, addr);
    if (fd >= 0)
    {
        returnAddr.setSockAddrIn(addr);
    }
    return fd;
}
void Socket::shutdownWrite()
{
    socketDetail::shutdownWrite(_sockfd);
}
