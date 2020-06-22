/*
 * File: echoServerm.cpp
 * ---------------------
 * @author: Fu Wei
 * simple I/O multiplexing echo server demo.
 * Process input from stdin and clients, but it will not respond to the commands in stdin until
 * the client closes the connection.
 */

#include <sys/select.h>
#include <iostream>
#include <io/robustio.hpp>
#include "network/socket.hpp"
#include "system/unixUtility.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
using namespace std;

const int MAXLINE = 8192;

void doCommand();
void echo(int connfd);
int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << "<port>\n";
        exit(0);
    }

    int listenfd = OpenListenFd(argv[1]);

    fd_set readSet, readySet;
    FD_ZERO(&readSet);
    FD_SET(STDIN_FILENO, &readSet);
    FD_SET(listenfd, &readSet);

    struct sockaddr clientaddr;

    while (true)
    {
        readySet = readSet;
        Select(listenfd + 1, &readySet, nullptr, nullptr, nullptr);
        if (FD_ISSET(STDIN_FILENO, &readySet))
        {
            doCommand();
        }

        if (FD_ISSET(listenfd, &readySet))
        {
            socklen_t clientlen = sizeof(struct sockaddr);
            int connfd = Accept(listenfd, &clientaddr, &clientlen);
            echo(connfd);
            Close(connfd);
        }
    }
}

void doCommand(void)
{
    string command;
    cin >> command;
    if (command == "exit")
    {
        exit(0);
    }
    cout << "Command: " << command << endl;
}

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

#pragma clang diagnostic pop