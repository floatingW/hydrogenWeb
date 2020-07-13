/*
 * File: EventLoop_test.cpp
 * ------------------------
 * @author: Fu Wei
 *
 */

#include "network/EventLoop.hpp"
#include "network/Channel.hpp"

#include <thread>
#include <iostream>

#include <sys/timerfd.h>
#include <strings.h>

EventLoop* g_loop;

void timeout()
{
    std::cout << "timeout!\n";
    g_loop->quit();
}

int main(int argc, char* argv[])
{
    EventLoop loop;
    g_loop = &loop;

    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    Channel channel(&loop, timerfd);
    channel.setReadCallBack(timeout);
    channel.enableReading();

    struct itimerspec howlong;
    ::bzero(&howlong, sizeof howlong);
    howlong.it_value.tv_sec = 3;
    ::timerfd_settime(timerfd, 0, &howlong, nullptr);

    loop.loop();

    ::close(timerfd);

    return 0;
}
