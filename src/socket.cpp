/*
 * File: socket.cpp
 * ----------------
 * @author: Fu Wei
 * The implementation of wrappers of system calls of sockets and client/server helpers.
 * - wrapper functions: with same behaviours and additional printing messages feature.
 * - client/server helpers: high level wrappers of socket functions that provides convenience
 *   and are protocol-independent.
 */

#include "network/socket.hpp"
#include "system/unixUtility.hpp"
#include <cstring>
#include <iostream>

using namespace std;
const int LISTENBACKLOG = 1024;

int Socket(int domain, int type, int protocol)
{
    int fd = socket(domain, type, protocol);
    if (fd < 0)
    {
        unix_error("Socket error");
    }
    return fd;
}

int Connect(int cliendfd, struct sockaddr* addr, socklen_t addrlen)
{
    int ret = connect(cliendfd, addr, addrlen);
    if (ret < 0)
    {
        unix_error("Connect error");
    }
    return ret;
}

int OpenClientFd(char* hostname, char* port)
{
    int clientfd = openClientFd(hostname, port);
    if (clientfd < 0)
    {
        unix_error("OpenClientFd failed");
    }
    return clientfd;
}

int Bind(int sockfd, struct sockaddr* addr, socklen_t addrlen)
{
    int ret = bind(sockfd, addr, addrlen);
    if (ret < 0)
    {
        unix_error("Bind error");
    }
    return ret;
}

int Listen(int sockfd, int backlog)
{
    int ret = listen(sockfd, backlog);
    if (ret < 0)
    {
        unix_error("Listen error");
    }
    return ret;
}

int Accept(int listenfd, struct sockaddr* addr, socklen_t* addrlen)
{
    int connfd = accept(listenfd, addr, addrlen);
    if (connfd < 0)
    {
        unix_error("Accept error");
    }
    return connfd;
}

int OpenListenFd(char* port)
{
    int listenfd = openListenFd(port);
    if (listenfd < 0)
    {
        unix_error("OpenListenFd failed");
    }
    return listenfd;
}

int GetAddrInfo(const char* host, const char* service, struct addrinfo* hints, struct addrinfo** result)
{
    int ret = getaddrinfo(host, service, hints, result);
    if (ret != 0)
    {
        gai_error(ret, "GetAddrInfo error");
    }
    return ret;
}

void FreeAddrInfo(struct addrinfo* result)
{
    freeaddrinfo(result);
}

int GetNameInfo(const struct sockaddr* addr, socklen_t addrlen, char* host, socklen_t hostlen, char* service, socklen_t servlen, int flags)
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
static int openClientFd(char* hostname, char* port)
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

static int openListenFd(char* port)
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
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int));

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
