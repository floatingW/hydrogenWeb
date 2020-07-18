/*
 * File: echoServerm.cpp
 * ---------------------
 * @author: Fu Wei
 * simple I/O multiplexing based event-driven echo server demo.
 * Process input from stdin and clients.
 */

#include <sys/select.h>
#include <iostream>
#include <io/robustio.hpp>
#include "network/socketDetail.hpp"
#include "system/unixUtility.hpp"
#include <unordered_map>
#include <memory>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

using namespace std;
using namespace socketDetail;

typedef struct
{
    unordered_map<int, shared_ptr<Riob>> fds;
    fd_set readSet;
    fd_set readySet;
    int maxfd;
} clientPool;

const int MAXLINE = 8192;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << "<port>\n";
        exit(0);
    }

    // listen on given port
    int listenfd = OpenListenFd(argv[1]);

    // pool initialization
    clientPool pool;
    FD_ZERO(&pool.readSet);
    FD_SET(listenfd, &pool.readSet);
    FD_SET(STDIN_FILENO, &pool.readSet);
    pool.fds[STDIN_FILENO] = make_shared<Riob>(STDIN_FILENO);
    pool.maxfd = listenfd;

    struct sockaddr clientaddr;

    ssize_t byte_count = 0;
    char usrbuf[MAXLINE];
    while (true)
    {
        pool.readySet = pool.readSet;
        Select(pool.maxfd + 1, &pool.readySet, nullptr, nullptr, nullptr);

        // if new connection coming
        if (FD_ISSET(listenfd, &pool.readySet))
        {
            socklen_t clientlen = sizeof(struct sockaddr);
            int connfd = Accept(listenfd, &clientaddr, &clientlen);

            // updates pool
            FD_SET(connfd, &pool.readSet);
            pool.maxfd = max(connfd, pool.maxfd);
            pool.fds[connfd] = make_shared<Riob>(connfd);
        }

        // echo each ready connected descriptor
        for (auto& fd : pool.fds)
        {
            // only echo the ready fds
            if (FD_ISSET(fd.first, &pool.readySet))
            {
                int n = fd.second->readlineb(usrbuf, MAXLINE);
                if (n > 0)
                {
                    byte_count += n;
                    cout << "server received " << n << " bytes (total: " << byte_count << ")";
                    cout << " on fd " << fd.first << "\n";
                    Rio::writen(fd.first, usrbuf, n);
                }
                else if (n == 0) // EOF
                {
                    Close(fd.first);
                    FD_CLR(fd.first, &pool.readSet);
                }
                else // read error
                {
                    cout << "read from fd " << fd.first << " failed!\n";
                }
            }
        }
    }
}

#pragma clang diagnostic pop