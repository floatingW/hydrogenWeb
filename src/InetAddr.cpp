/*
 * File: InetAddr.cpp
 * ---------------------------------
 * @author: Fu Wei
 * Implementation of InetAddr class.
 */

#include "network/InetAddr.hpp"
#include "system/unixUtility.hpp"

#include <cstring> // memset()

#include <arpa/inet.h> // htonl(), htons(), ntohs(), inet_ntop(), inet_pton()

InetAddr::InetAddr(uint16_t port)
{
    // TODO: refactor
    ::memset(&_addr, 0, sizeof _addr);
    _addr.sin_family = AF_INET; // for internet connection, is always AF_INET
    _addr.sin_port = ::htons(port);
    _addr.sin_addr.s_addr = ::htonl(INADDR_ANY); // address to accept any incoming messages
}

InetAddr::InetAddr(const std::string& ip, uint16_t port)
{
    // TODO: refactor
    ::memset(&_addr, 0, sizeof _addr);
    _addr.sin_family = AF_INET; // for internet connection, is always AF_INET
    _addr.sin_port = htons(port);
    if (::inet_pton(AF_INET, ip.c_str(), &_addr.sin_addr) <= 0)
    { // ip is invalid or error happened
        unix_error("inet_pton error");
    }
}
std::string InetAddr::toString() const
{
    std::string host = "illegal";
    char host_buf[INET_ADDRSTRLEN];
    auto ret = ::inet_ntop(AF_INET, &_addr.sin_addr, host_buf, sizeof host_buf);

    if (ret)
    {
        host = host_buf;
    }
    uint16_t port = ::ntohs(_addr.sin_port);

    return host + ":" + std::to_string(port);
}
