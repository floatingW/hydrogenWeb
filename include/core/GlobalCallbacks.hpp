/*
 * File: GlobalCallbacks.hpp
 * -------------------------
 * @author: Fu Wei
 *
 */

#ifndef HYDROGENWEB_GLOBALCALLBACKS_HPP
#define HYDROGENWEB_GLOBALCALLBACKS_HPP

#include "core/Timestamp.hpp"

#include <functional> // function
#include <memory> // shared_ptr

class HyBuffer;
class TcpConnection;
typedef std::shared_ptr<TcpConnection> pTcpConnection;

typedef std::function<void(const pTcpConnection&)> ConnectionCallback;
typedef std::function<void(const pTcpConnection&,
                           HyBuffer* buf,
                           Timestamp receiveTime)>
    MessageCallback;
typedef std::function<void(const pTcpConnection&)> CloseCallback;

#endif //HYDROGENWEB_GLOBALCALLBACKS_HPP
