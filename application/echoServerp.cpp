/*
 * File: echoServerp.cpp
 * ---------------------
 * @author: Fu Wei
 * simple multi-process echo server demo
 */

#include "system/unixUtility.hpp"
#include "network/socket.hpp"
#include "io/robustio.hpp"
#include "io/sio.hpp"
#include <iostream>
#include <sys/wait.h>

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

/*
 * SIGCHLD handler
 */
void sigchld_handler(int signum)
{
    pid_t pid;

    while ((pid = waitpid(-1, nullptr, WNOHANG)) > 0)
    {
        std::string msg = "retrieve child process " + std::to_string(pid) + "\n";
        sio::Prints(msg);
    }
}
int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << "<port>\n";
        exit(0);
    }

    Signal(SIGCHLD, sigchld_handler);

    char* port = argv[1];

    char client_hostname[MAXLINE], client_port[MAXLINE];
    struct sockaddr clientaddr;

    int listenfd = OpenListenFd(port);
    while(true) {
        socklen_t clientlen = sizeof(struct sockaddr);
        int connfd = Accept(listenfd, &clientaddr, &clientlen);
        if (Fork() == 0) {
            Close(listenfd);
            echo(connfd);
            Close(connfd);
            exit(0);
        }
        Close(connfd);
    }
}

#pragma clang diagnostic pop