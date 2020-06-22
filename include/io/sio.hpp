/*
 * File: sio.hpp
 * -------------
 * @author: Fu Wei
 * The Signal-safe I/O package. Providing reentrant output functions for signal handlers.
 */

#ifndef HYDROGENWEB_SIO_HPP
#define HYDROGENWEB_SIO_HPP

#include <sys/types.h>
#include <unistd.h>
#include <string>

namespace sio
{
    /*
     * wrapper functions
     */
    ssize_t Printl(long v);
    ssize_t Prints(const std::string& msg);
    ssize_t Prints(const char s[]);

    void Error(const char s[]);

}
#endif //HYDROGENWEB_SIO_HPP
