//
// Created by fwei on 6/20/20.
//
#include "system/unixUtility.hpp"
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <netdb.h>
#include <cerrno>

using namespace std;

void unix_error(const string& msg)
{
    cerr << msg << ": " << strerror(errno) << endl;
}

void posix_error(int code, const string& msg)
{
    cerr << msg << ": " << strerror(code) << endl;
}

void gai_error(int code, const string& msg)
{
    cerr << msg << ": " << gai_strerror(code) << endl;
}

void app_error(const string& msg)
{
    cerr << msg << endl;
}

int Close(int fd)
{
    int ret = close(fd);
    if (ret < 0)
    {
        unix_error("Close error");
    }
    return ret;
}
