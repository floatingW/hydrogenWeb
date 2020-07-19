/*
 * File: Acceptor_test.cpp
 * -----------------------
 * @author: Fu Wei
 * listen 2 ports, each port with different service.
 */

#include <iostream>
#include "network/EventLoop.hpp"
#include "network/Acceptor.hpp"
#include "network/InetAddr.hpp"
#include "network/Socket.hpp"

using namespace std;

void sayHello(Socket&& newSocket, const InetAddr& returnaddr)
{
    const Socket& s = newSocket;
    cout << "new connection from " << returnaddr.toString() << endl;
    ::write(s.fd(), "hello~\n", 7);
}

void sayHi(Socket&& newSocket, const InetAddr& returnaddr)
{
    const Socket& s = newSocket;
    cout << "new connection from " << returnaddr.toString() << endl;
    ::write(s.fd(), "hi~\n", 4);
}

int main(int argc, char* argv[])
{
    cout << "in main(): pid = " << gettid() << endl;
    InetAddr addr1(23456);
    InetAddr addr2(23457);
    EventLoop loop;

    Acceptor acceptor1(&loop, addr1);
    Acceptor acceptor2(&loop, addr2);

    acceptor1.setIncomingCallBack(sayHello);
    acceptor2.setIncomingCallBack(sayHi);

    acceptor1.listen();
    acceptor2.listen();
    loop.loop();

    return 0;
}
