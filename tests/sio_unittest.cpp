//
// Created by fwei on 6/19/20.
//
#include "io/sio.hpp"
#include "sio.cpp" // work around for static functions
#include "gtest/gtest.h"
#include <string>

using namespace std;
TEST(sio_reverse, simple_input)
{
    // simple string
    string str = "Reverse a string (from K&R). ";
    string strCopy(str);
    char* s = strCopy.data();

    sio::_reverse(s);
    std::reverse(str.begin(), str.end());

    ASSERT_STREQ(s, str.c_str());

    sio::_reverse(s);
    std::reverse(str.begin(), str.end());

    ASSERT_STREQ(s, str.c_str());

    // empty string
    str = "";
    strCopy = str;
    s = strCopy.data();

    sio::_reverse(s);
    std::reverse(str.begin(), str.end());

    ASSERT_STREQ(s, str.c_str());

    sio::_reverse(s);
    std::reverse(str.begin(), str.end());

    ASSERT_STREQ(s, str.c_str());
}

TEST(sio_ltoa, simple_input)
{
    int n = 10000;
    char s[128];
    string iStr;
    for (int i = -n; i < n; i++)
    {
        sio::_ltoa(i, s, 10);
        iStr = to_string(i);
        EXPECT_EQ(string(s), iStr);
    }

    sio::_ltoa(INT64_MAX, s, 10);
    iStr = to_string(INT64_MAX);
    EXPECT_EQ(string(s), iStr);

    /*
     * Due to the range of long integer, the smallest negative number will cause an overflow.
     * Because this situation is rare, just ignore this special case.
     */
    sio::_ltoa(INT64_MIN + 1, s, 10);
    iStr = to_string(INT64_MIN + 1);
    EXPECT_EQ(string(s), iStr);
}

TEST(sio_strlen, simple_input)
{
    // simple string
    string str = "cowboy bebop in the future";
    std::size_t len_sio = sio::_strlen(str.c_str());
    EXPECT_EQ(len_sio, str.length());

    // empty string
    str = "";
    len_sio = sio::_strlen(str.c_str());
    EXPECT_EQ(len_sio, str.length());
}

TEST(sio_print_functions, simple_output) {
    std::string msg = "print some messages\n";
    std::string errMsg = "print some errors\n";

    sio::Printl(1234567890);
    sio::Prints(std::string("\n").c_str());
    sio::Prints(msg.c_str());
    sio::Error(errMsg.c_str());

}
