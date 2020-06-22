/*
 * File: echoServert.cpp
 * ---------------------
 * @author: Fu Wei
 * simple multi-thread echo server demo.
 * Just for linux, so Posix thread is Ok.
 */

#include <pthread.h>
#include "system/unixUtility.hpp"
#include "network/socket.hpp"
#include "io/robustio.hpp"
#include "io/sio.hpp"
#include <iostream>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
using namespace std;

const int MAXLINE = 8192;

void echo(int connfd)
{
    size_t n;
    char buf[MAXLINE];
    Riob riob(connfd);

    while ((n = riob.readlineb(buf, MAXLINE)) != 0)
    {
        cout << "server received " << n << " bytes\n";
        Rio::writen(connfd, buf, n);
    }
}

void *echoThread(void *argp) {
    int connfd = *(int *)argp;
    Pthread_detach(Pthread_self());
    delete (int*)argp; // we need to free the memory of connfd pointer

    echo(connfd);
    Close(connfd);

    cout << "thread " << Pthread_self() << " finished\n";
    return nullptr;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << "<port>\n";
        exit(0);
    }

    char* port = argv[1];

    char client_hostname[MAXLINE], client_port[MAXLINE];
    struct sockaddr clientaddr;

    int listenfd = OpenListenFd(port);

    while (true) {
        socklen_t clientlen = sizeof(struct sockaddr);
        int *connfdp = new int;
        *connfdp = Accept(listenfd, &clientaddr, &clientlen);
        pthread_t pid;
        Pthread_create(&pid, nullptr, echoThread, connfdp);
    }

    return 0;
}

#pragma clang diagnostic pop