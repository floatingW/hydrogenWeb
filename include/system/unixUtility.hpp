//
// Created by fwei on 6/20/20.
//

#ifndef HYDROGENWEB_UNIXUTILITY_HPP
#define HYDROGENWEB_UNIXUTILITY_HPP

#include <cstdio>
#include <string>

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
#endif //HYDROGENWEB_UNIXUTILITY_HPP
