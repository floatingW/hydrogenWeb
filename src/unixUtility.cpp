//
// Created by fwei on 6/20/20.
//
#include "system/unixUtility.hpp"
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <netdb.h>
#include <cerrno>
#include <sys/wait.h>

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

/*
 * wrappers for Unix I/O routines
 */
int Close(int fd)
{
    int ret = close(fd);
    if (ret < 0)
    {
        unix_error("Close error");
    }
    return ret;
}

int Select(int n, fd_set* readfds, fd_set* writefds, fd_set* errorfds, struct timeval* timeout)
{
    int ret = select(n, readfds, writefds, errorfds, timeout);
    if (ret < 0)
    {
        unix_error("Select error");
    }
    return ret;
}

pid_t Fork()
{
    pid_t pid;

    if ((pid = fork()) < 0)
    {
        unix_error("Fork error");
    }
    return pid;
}

pid_t Waitpid(pid_t pid, int* stat_loc, int options)
{
    pid_t ret;

    if ((ret = waitpid(pid, stat_loc, options)) < 0)
    {
        unix_error("Waitpid error");
    }

    return ret;
}

/*
 * Signal - wrapper for sigaction
 * From Unix Network Programming: The Sockets Networking API (W.R.Stevens).
 */
handler_t* Signal(int signum, handler_t* handler)
{
    struct sigaction action, actionOld;

    action.sa_handler = handler;
    Sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART; // restart syscalls if possible

    if (sigaction(signum, &action, &actionOld) < 0)
    {
        unix_error("Signal error");
    }
    return actionOld.sa_handler;
}

int Sigemptyset(sigset_t* set)
{
    int ret = sigemptyset(set);
    if (ret < 0)
    {
        unix_error("Sigemptyset error");
    }
    return ret;
}

int Sigfillset(sigset_t* set)
{
    int ret = sigfillset(set);
    if (ret < 0)
    {
        unix_error("Sigfillset error");
    }
    return ret;
}

int Sigaddset(sigset_t* set, int signum)
{
    int ret = sigaddset(set, signum);
    if (ret < 0)
    {
        unix_error("Sigaddset error");
    }
    return ret;
}

int Pthread_create(pthread_t* tid, const pthread_attr_t* attr, func* f, void* arg)
{
    int ret = pthread_create(tid, attr, f, arg);
    if (ret != 0)
    {
        posix_error(ret, "Pthread_create error");
    }
    return ret;
}

int Pthread_detach(pthread_t pid)
{
    int ret = pthread_detach(pid);
    if (ret != 0)
    {
        posix_error(ret, "Pthread_detach error");
    }
    return ret;
}

/*
 * Pthread_self - this function always succeeds (from Linux Programmer's manual). So we just
 * return its original return value without any error handling.
 */
pthread_t Pthread_self()
{
    return pthread_self();
}
