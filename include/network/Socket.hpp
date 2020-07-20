/*
 * File: Socket.hpp
 * ----------------
 * @author: Fu Wei
 * Interface of a RAII-style socketfd wrapper class.
 */

#ifndef HYDROGENWEB_SOCKET_HPP
#define HYDROGENWEB_SOCKET_HPP

class InetAddr;

class Socket
{
public:
    explicit Socket(const int sockfd) :
        _sockfd(sockfd) {}

    ~Socket();

    int fd() const { return _sockfd; }

    /*
     * option setting
     */
    void setReuse(bool reuse) const;
    void bindAddr(const InetAddr& listenAddr) const;
    void listen() const;
    int accept(InetAddr& addr);

    /*
     * movable
     */
    Socket(Socket&& s) noexcept :
        _sockfd(s._sockfd)
    {
        s._sockfd = -1;
    }

    Socket& operator=(Socket&& rhs) noexcept
    {
        if (this != &rhs)
        {
            _sockfd = rhs._sockfd;
            rhs._sockfd = -1;
        }
        return *this;
    }

private:
    int _sockfd;
};

#endif //HYDROGENWEB_SOCKET_HPP
