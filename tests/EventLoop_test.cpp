/*
 * File: EventLoop_test.cpp
 * ------------------------
 * @author: Fu Wei
 *
 */

#include "network/EventLoop.hpp"
#include "core/TimeStamp.hpp"

#include <thread>
#include <iostream>
#include <string>

#include <sys/timerfd.h>
#include <strings.h>

using namespace std;

EventLoop* g_loop;
int cnt1 = 0;
int cnt2 = 0;
//int cnt3 = 0;
int cnt4 = 0;

void print1(string s)
{
    cout << "pid = " << getpid() << ", tid = " << gettid() << endl;
    cout << TimeStamp::now().toString() << " " << s << endl;
    if (cnt1++ >= 6)
    {
        g_loop->quit();
    }
}

void print2(string s)
{
    cout << "pid = " << getpid() << ", tid = " << gettid() << endl;
    cout << TimeStamp::now().toString() << " " << s << endl;
    if (cnt2++ >= 8)
    {
        g_loop->quit();
    }
}

void print3(string s)
{
    cout << "pid = " << getpid() << ", tid = " << gettid() << endl;
    cout << TimeStamp::now().toString() << " " << s << endl;
}

void print4(string s)
{
    cout << "pid = " << getpid() << ", tid = " << gettid() << endl;
    cout << TimeStamp::now().toString() << " " << s << endl;
    if (cnt4++ >= 15)
    {
        g_loop->quit();
    }
}

void loop1()
{
    EventLoop loop;
    g_loop = &loop;

    print1("in loop1: adding timers in reverse order with some duplicate timers");
    loop.runAfter(4.0, std::bind(print1, "after 4.0"));
    loop.runAfter(4.0, std::bind(print1, "after 4.0"));
    loop.runAfter(4.0, std::bind(print1, "after 4.0"));
    loop.runAfter(3.0, std::bind(print1, "after 3.0"));
    loop.runAfter(2.0, std::bind(print1, "after 2.0"));
    loop.runAfter(1.0, std::bind(print1, "after 1.0"));

    loop.loop();
    cout << "eventloop exited" << endl;
}

void loop2()
{
    EventLoop loop;
    g_loop = &loop;

    print2("in loop2: adding timers in order with some duplicate timers");
    loop.runAfter(1.0, std::bind(print2, "after 1.0"));
    loop.runAfter(1.0, std::bind(print2, "after 1.0"));
    loop.runAfter(1.0, std::bind(print2, "after 1.0"));
    loop.runAfter(2.0, std::bind(print2, "after 2.0"));
    loop.runAfter(3.0, std::bind(print2, "after 3.0"));
    loop.runAfter(4.0, std::bind(print2, "after 4.0"));
    loop.runAfter(4.0, std::bind(print2, "after 4.0"));
    loop.runAfter(4.0, std::bind(print2, "after 4.0"));

    loop.loop();
    cout << "eventloop exited" << endl;
}

void loop3()
{
    EventLoop loop;
    g_loop = &loop;

    print2("in loop3: adding timers in order with repeat timers");
    loop.runAfter(1.0, std::bind(print4, "after 1.0"));
    loop.runEvery(1.0, std::bind(print4, "repeat 1.0"));
    loop.runAfter(2.0, std::bind(print4, "after 2.0"));
    loop.runAfter(3.0, std::bind(print4, "after 3.0"));
    loop.runAfter(4.0, std::bind(print4, "after 4.0"));

    loop.loop();
    cout << "eventloop exited" << endl;
}

void loop4()
{
    EventLoop loop;
    g_loop = &loop;

    print3("in loop4: no timer");

    loop.loop();
    g_loop->quit();
    cout << "eventloop exited" << endl;
}

void loopAll()
{
    loop1();
    loop2();
    loop3();
    loop4();
}

int main(int argc, char* argv[])
{
    loopAll();
    return 0;
}
