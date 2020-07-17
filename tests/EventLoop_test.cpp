/*
 * File: EventLoop_test.cpp
 * ------------------------
 * @author: Fu Wei
 *
 */

#include "network/EventLoop.hpp"
#include "core/EventLoopThread.hpp"
#include "core/TimeStamp.hpp"

#include <thread>
#include <iostream>
#include <string>

#include <sys/timerfd.h>
#include <strings.h>

using namespace std;

EventLoop* g_loop;
int g_flag = 0;

void fun4()
{
    cout << "in run4(): pid = " << getpid() << ", g_flag = " << g_flag << endl;
}
void fun3()
{
    cout << "in run3(): pid = " << getpid() << ", g_flag = " << g_flag << endl;
    g_loop->runAfter(2, fun4);
    g_flag++;
}
void fun2()
{
    cout << "in run2(): pid = " << getpid() << ", g_flag = " << g_flag << endl;
    g_loop->addToLoopThread(fun3);
}
void fun1()
{
    cout << "in run1(): pid = " << getpid() << ", g_flag = " << g_flag << endl;
    g_loop->runInLoopThread(fun2);
    g_flag++;
}

void fun0()
{
    g_loop->runAfter(1.0, fun1);
}

int main(int argc, char* argv[])
{
    cout << "in main(): pid = " << gettid() << ", g_flag = " << g_flag << endl;
    EventLoopThread loopThread;
    g_loop = loopThread.start();

    g_loop->runAfter(1.0, fun0);

    return 0;
}
