/*
 * File: GlobalCallbacks.hpp
 * -------------------------
 * @author: Fu Wei
 *
 */

#ifndef HYDROGENWEB_GLOBALCALLBACKS_HPP
#define HYDROGENWEB_GLOBALCALLBACKS_HPP

#include <functional> // function
#include <memory> // shared_ptr

class TcpConnection;
typedef std::shared_ptr<TcpConnection> pTcpConnection;

typedef std::function<void(const pTcpConnection&)> ConnectionCallback;
typedef std::function<void(const pTcpConnection&,
                           const char* msg,
                           ssize_t length)>
    MessageCallback;
typedef std::function<void(const pTcpConnection&)> CloseCallback;

#endif //HYDROGENWEB_GLOBALCALLBACKS_HPP
