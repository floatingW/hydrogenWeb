/*
 * File: echoServert.cpp
 * ---------------------
 * @author: Fu Wei
 * Simple prethreading based event-driven echo server demo.
 * Just for linux, so Posix thread is Ok.
 */

#include <pthread.h>
#include "system/unixUtility.hpp"
#include "network/socket.hpp"
#include "io/robustio.hpp"
#include "io/sio.hpp"
#include <iostream>
#include <mutex>
#include "sem/sharedBuf.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
using namespace std;

const int MAXLINE = 8192;
const int NTHREADS = 10;
SharedBuf buf(10);

size_t byte_cnt = 0;
mutex mtx;

void echo(int connfd);
void* echoThread(void* argp);

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << "<port>\n";
        exit(0);
    }

    char* port = argv[1];

    struct sockaddr clientaddr;

    int listenfd = OpenListenFd(port);

    // create worker threads
    for (int i = 0; i < NTHREADS; i++)
    {
        pthread_t pid;
        Pthread_create(&pid, nullptr, echoThread, nullptr);
    }

    while (true)
    {
        socklen_t clientlen = sizeof(struct sockaddr);
        int connfd = Accept(listenfd, &clientaddr, &clientlen);
        buf.insert(connfd);
    }
}

void echo(int connfd)
{
    size_t n;
    char buf[MAXLINE];
    Riob riob(connfd);

    while ((n = riob.readlineb(buf, MAXLINE)) != 0)
    {
        unique_lock<mutex> lock(mtx);
        byte_cnt += n;
        cout << "server received " << n << " bytes (total: " << byte_cnt << " bytes)\n";
        lock.unlock();
        Rio::writen(connfd, buf, n);
    }
}

void* echoThread(void* argp)
{
    Pthread_detach(Pthread_self());

    while (true)
    {
        int connfd = buf.remove();
        echo(connfd);
        Close(connfd);
    }

    return nullptr;
}

#pragma clang diagnostic pop