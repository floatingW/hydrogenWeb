//
// Created by fwei on 6/20/20.
//
#include "system/unixUtility.hpp"
#include "network/socketDetail.hpp"
#include "io/robustio.hpp"
#include <iostream>

#include <cstdio>

using namespace std;

const int MAXLINE = 8192;
int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        cerr << "Usage: " << argv[0] << "<host> <port>\n";
        exit(0);
    }

    char* host = argv[1];
    char* port = argv[2];
    int clientfd = OpenClientFd(host, port);

    Riob riob(clientfd);
    char usrbuf[MAXLINE];
    int n;
    while (fgets(usrbuf, MAXLINE, stdin) != 0)
    {
        Rio::writen(clientfd, usrbuf, strlen(usrbuf));
        riob.readlineb(usrbuf, MAXLINE);
        cout << usrbuf;
    }

    Close(clientfd);
    return 0;
}