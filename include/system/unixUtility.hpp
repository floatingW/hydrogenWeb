//
// Created by fwei on 6/20/20.
//

#ifndef HYDROGENWEB_UNIXUTILITY_HPP
#define HYDROGENWEB_UNIXUTILITY_HPP

#include <cstdio>
#include <string>
#include <unistd.h>
#include <csignal>
#include <pthread.h>

/*
 * error handling
 */
void unix_error(const std::string& msg);
void posix_error(int code, const std::string& msg);
void gai_error(int code, const std::string& msg);
void app_error(const std::string& msg);

/*
 * wrappers for Unix I/O routines
 */
int Close(int fd);

/*
 * wrappers for Unix process control functions
 */
pid_t Fork();
pid_t Waitpid(pid_t pid, int *stat_loc, int options);

/*
 * wrappers for Unix signal functions
 */
typedef void handler_t(int); // define a signal handler type
handler_t *Signal(int signum, handler_t *handler);
int Sigemptyset(sigset_t *set);
int Sigfillset(sigset_t *set);
int Sigaddset(sigset_t *set, int signum);

/*
 * wrappers for Pthread control functions
 */
typedef void *(func)(void *); // thread routine, a function without return value and args
int Pthread_create(pthread_t *tid, const pthread_attr_t *attr, func *f, void *arg);
int Pthread_detach(pthread_t pid);
pthread_t Pthread_self();
#endif //HYDROGENWEB_UNIXUTILITY_HPP
