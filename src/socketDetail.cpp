/*
 * File: socket.cpp
 * ----------------
 * @author: Fu Wei
 * The implementation of wrappers of system calls of sockets and client/server helpers.
 * - wrapper functions: with same behaviours and additional printing messages feature.
 * - client/server helpers: high level wrappers of socket functions that provides convenience
 *   and are protocol-independent.
 */

#include "network/socketDetail.hpp"
#include "system/unixUtility.hpp"
#include <cstring>
#include <iostream>

using namespace std;

const int LISTENBACKLOG = 1024;

// TODO: refactor all

int socketDetail::Socket(int domain, int type, int protocol)
{
    int fd = socket(domain, type, protocol);
    if (fd < 0)
    {
        unix_error("Socket error");
    }
    return fd;
}

int socketDetail::Connect(int cliendfd, const struct sockaddr& addr)
{
    int ret = connect(cliendfd, &addr, sizeof addr);
    if (ret < 0)
    {
        unix_error("Connect error");
    }
    return ret;
}

int socketDetail::OpenClientFd(char* hostname, char* port)
{
    int clientfd = openClientFd(hostname, port);
    if (clientfd < 0)
    {
        unix_error("OpenClientFd failed");
    }
    return clientfd;
}

int socketDetail::Bind(int sockfd, const struct sockaddr& addr)
{
    int ret = bind(sockfd, &addr, sizeof addr);
    if (ret < 0)
    {
        unix_error("Bind error");
        abort(); // should abort
    }
    return ret;
}

int socketDetail::Bind(int sockfd, const struct sockaddr_in& addr)
{
    int ret = bind(sockfd, reinterpret_cast<const struct sockaddr*>(&addr), sizeof addr);
    if (ret < 0)
    {
        unix_error("Bind error");
        abort(); // should abort
    }
    return ret;
}

int socketDetail::Listen(int sockfd, int backlog)
{
    int ret = listen(sockfd, backlog);
    if (ret < 0)
    {
        unix_error("Listen error");
        abort(); // should abort
    }
    return ret;
}

int socketDetail::Accept(int listenfd, struct sockaddr& addr)
{
    socklen_t addrlen = sizeof addr;
    int connfd = accept(listenfd, &addr, &addrlen);
    if (connfd < 0)
    {
        unix_error("Accept error");
        // TODO: check errno
    }
    return connfd;
}

int socketDetail::Accept(int listenfd, struct sockaddr_in& addr) // for class Socket
{
    socklen_t addrlen = sizeof addr;
    int connfd = accept4(listenfd,
                         reinterpret_cast<struct sockaddr*>(&addr),
                         &addrlen,
                         SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd < 0)
    {
        unix_error("Accept error");
        // TODO: check errno case-by-case
    }
    return connfd;
}

int socketDetail::OpenListenFd(char* port)
{
    int listenfd = openListenFd(port);
    if (listenfd < 0)
    {
        unix_error("OpenListenFd failed");
    }
    return listenfd;
}

int socketDetail::GetAddrInfo(const char* host, const char* service, const struct addrinfo* hints, struct addrinfo** result)
{
    int ret = getaddrinfo(host, service, hints, result);
    if (ret != 0)
    {
        gai_error(ret, "GetAddrInfo error");
    }
    return ret;
}

void socketDetail::FreeAddrInfo(struct addrinfo* result)
{
    freeaddrinfo(result);
}

int socketDetail::GetNameInfo(const struct sockaddr* addr, socklen_t addrlen, char* host, socklen_t hostlen, char* service, socklen_t servlen, int flags)
{
    int ret = getnameinfo(addr, addrlen, host, hostlen, service, servlen, flags);
    if (ret != 0)
    {
        gai_error(ret, "GetNameInfo error");
    }
    return ret;
}

/*
 * client/server helper
 */
int socketDetail::openClientFd(char* hostname, char* port)
{
    int clientfd, ret;
    struct addrinfo hints, *listp, *p;
    memset(&hints, 0, sizeof(struct addrinfo));

    // Get a list of addrinfo records
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;
    hints.ai_flags |= AI_ADDRCONFIG;
    if ((ret = getaddrinfo(hostname, port, &hints, &listp)) != 0)
    {
        cerr << "getaddrinfo failed (" << hostname << ":" << port << "): "
             << gai_strerror(ret) << endl;
        return -2;
    }

    // Loop the addrinfo list and select one that can be connected successfully
    for (p = listp; p; p = p->ai_next)
    {
        if ((clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
        {
            continue;
        }

        if (connect(clientfd, p->ai_addr, p->ai_addrlen) != -1) // Find the correct
        {
            break;
        }

        if (Close(clientfd) < 0)
        {
            return -1;
        }
    }

    FreeAddrInfo(listp); // release memory of the addrinfo list

    if (!p) // All addrinfo failed
    {
        return -1;
    }
    else // Open a socket successfully
    {
        return clientfd;
    }
}

int socketDetail::openListenFd(char* port)
{
    int listenfd, ret, optval = 1;
    struct addrinfo hints, *listp, *p;
    memset(&hints, 0, sizeof(struct addrinfo));

    // Get a list of addrinfo records
    hints.ai_socktype = SOCK_STREAM; /* Accept connections */
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; /* ... on any IP address */
    hints.ai_flags |= AI_NUMERICSERV; /* ... using port number */
    if ((ret = getaddrinfo(NULL, port, &hints, &listp)) != 0)
    {
        cerr << "getaddrinfo failed (port " << port << "): " << gai_strerror(ret) << endl;
        return -2;
    }

    // Loop the addrinfo list and select one that can be bind to
    for (p = listp; p; p = p->ai_next)
    {
        if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
        {
            continue;
        }

        // Eliminates "Address already in use" error from bind
        Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int));

        /* Bind the descriptor to the address */
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
            break; /* Success */

        if (Close(listenfd) < 0)
        {
            return -1;
        }
    }

    FreeAddrInfo(listp); // release memory of addrinfo list
    if (!p)
    {
        return -1;
    }

    /* Make it a listening socket ready to accept connection requests */
    if (Listen(listenfd, LISTENBACKLOG) < 0)
    {
        Close(listenfd);
        return -1;
    }

    return listenfd;
}

int socketDetail::Setsockopt(int sockfd, int level, int optname, const void* optval, socklen_t optlen)
{
    int ret = ::setsockopt(sockfd, level, optname, optval, optlen);
    if (ret < 0)
    {
        unix_error("Setsockopt error");
    }
    return ret;
}

int socketDetail::nonblockingSocket()
{
    int sockfd = Socket(AF_INET, // always AF_INET, for internet connection
                        SOCK_STREAM | // tcp socket
                            SOCK_NONBLOCK | // non-blocking IO
                            SOCK_CLOEXEC, // close this fd after exec()
                        IPPROTO_TCP);
    if (sockfd < 0)
    {
        abort();
    }
    return sockfd;
}

struct sockaddr_in socketDetail::getLocalAddr(int sockfd)
{
    struct sockaddr_in localAddr;
    ::memset(&localAddr, 0, sizeof localAddr);

    socklen_t addrlen = sizeof localAddr;
    if (::getsockname(sockfd, reinterpret_cast<struct sockaddr*>(&localAddr), &addrlen) < 0)
    {
        unix_error("getLocalAddr error");
    }

    return localAddr;
}

void socketDetail::shutdownWrite(int sockfd)
{
    if (::shutdown(sockfd, SHUT_WR) < 0) /** disables further send operations */
    {
        unix_error("socketDetail::shutdownWrite error");
    }
}
