/*
 * File: socket.hpp
 * ----------------
 * @author: Fu Wei
 * Provide socket interface wrappers that will print error message while
 * error happens and client/server socket helpers that are protocol-independent.
 *
 */

#ifndef HYDROGENWEB_SOCKET_HPP
#define HYDROGENWEB_SOCKET_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

/*
 * client socket interface wrappers
 */
int Socket(int domain, int type, int protocol);
int Connect(int cliendfd, const struct sockaddr* addr, socklen_t addrlen);

/*
 * client socket connection helper
 */
int OpenClientFd(char* hostname, char* port); // wrapper of openClientFd
static int openClientFd(char* hostname, char* port);

/*
 * server socket interface wrappers
 */
int Bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
int Listen(int sockfd, int backlog);
int Accept(int listenfd, struct sockaddr* addr, socklen_t* addrlen);

/*
 * server socket listening helper
 */
int OpenListenFd(char* port); // wrapper of openListenFd
static int openListenFd(char* port);

/*
 * Wrappers of protocol-independent socket helper
 */
int GetAddrInfo(const char* host, const char* service, const struct addrinfo* hints, struct addrinfo** result);

void FreeAddrInfo(struct addrinfo* result);

int GetNameInfo(const struct sockaddr* addr, socklen_t addrlen, char* host, socklen_t hostlen, char* service, socklen_t servlen, int flags);

#endif //HYDROGENWEB_SOCKET_HPP
