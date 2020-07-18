/*
 * File: echoServer.cpp
 * --------------------
 * @author: Fu Wei
 * simple iterative echo server demo
 */

#include "system/unixUtility.hpp"
#include "network/socketDetail.hpp"
#include "io/robustio.hpp"
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
    while (true)
    {
        socklen_t clientlen = sizeof(struct sockaddr);
        int connfd = Accept(listenfd, &clientaddr, &clientlen);
        GetNameInfo(&clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
        cout << "Connected to (" << client_hostname << ": " << client_port << ")\n";
        echo(connfd);
        Close(connfd);
    }
}

#pragma clang diagnostic pop