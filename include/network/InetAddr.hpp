/*
 * File: InetAddr.hpp
 * ------------------
 * @author: Fu Wei
 * Interface of a wrapper of struct sockaddr_in.
 */

#ifndef HYDROGENWEB_INETADDR_HPP
#define HYDROGENWEB_INETADDR_HPP

#include "network/socketDetail.hpp"

#include <string>

#include <netinet/in.h> // struct sockaddr_in, INADDR_ANY macro, INET_ADDRSTRLEN macro

class InetAddr
{
public:
    explicit InetAddr(uint16_t port);

    InetAddr(const std::string& ip, uint16_t port);

    explicit InetAddr(const struct sockaddr_in& addr) :
        _addr(addr)
    {
    }

    const struct sockaddr_in& getSockAddrIN() const { return _addr; }
    void setSockAddrIn(struct sockaddr_in& sockaddrIn) { _addr = sockaddrIn; }

    std::string toString() const;

private:
    struct sockaddr_in _addr;
};

#endif //HYDROGENWEB_INETADDR_HPP
