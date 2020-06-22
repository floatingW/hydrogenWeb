/*
 * File: sio.cpp
 * -------------
 * @author: Fu Wei
 * Implementation of SIO packages. Some private functions are coming from The C Programming
 * Language (K&R).
 */

#include "io/sio.hpp"

namespace sio
{
    /*
     * Forward declarations
     */
    static void _ltoa(long v, char* s, int b);
    static size_t _strlen(const char* s);
    static ssize_t printl(long v);
    static ssize_t prints(const char* s);
    static void error(const char* s);

    ssize_t Printl(long v)
    {
        ssize_t n = printl(v);
        if (n < 0)
        {
            error("Printl error");
        }
        return n;
    }

    /*
     * wrapper for Prints(char* s)
     */
    ssize_t Prints(const std::string& msg)
    {
        const char* msgCstyle = msg.c_str();
        return Prints(msgCstyle);
    }

    ssize_t Prints(const char* s)
    {
        ssize_t n = prints(s);
        if (n < 0)
        {
            error("Prints error");
        }
        return n;
    }

    void Error(const char* s)
    {
        error(s);
    }


    static ssize_t prints(const char* s)
    {
        return write(STDOUT_FILENO, s, _strlen(s));
    }

    static ssize_t printl(long v)
    {
        char s[128];

        _ltoa(v, s, 10);
        return prints(s);
    }

    static void error(const char* s)
    {
        prints(s);
    }

    /*
     * _reverse - Reverse string s (from The C Programming Language, K&R)
     */
    static void _reverse(char* s)
    {
        char c;

        for (int i = 0, j = _strlen(s) - 1; i < j; i++, j--)
        {
            c = s[i];
            s[i] = s[j];
            s[j] = c;
        }
    }

    /*
     * _ltoa - Convert long to base b string (from The C Programming Language, k&R)
     */
    static void _ltoa(long v, char* s, int b)
    {
        int c = 0;
        int i = 0;
        bool sign = v < 0;

        if (sign)
        {
            v = -v;
        }

        do
        {
            s[i++] = ((c = (v % b)) < 10) ? c + '0' : c - 10 + 'a';
        } while ((v /= b) > 0);

        if (sign)
        {
            s[i++] = '-';
        }

        s[i] = '\0';
        _reverse(s);
    }

    /*
     * _strlen - Return length of string s (from The C Programming Language, k&R)
     */
    static size_t _strlen(const char* s)
    {
        const char* p = s;

        while (*p != '\0')
        {
            p++;
        }
        return p - s;
    }
}
