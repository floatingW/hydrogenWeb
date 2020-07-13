/*
 * File: TimeStamp_unittest.cpp
 * ---------------------------------
 * @author: Fu Wei
 *
 */

#include "core/TimeStamp.hpp"
#include "gtest/gtest.h"

using namespace std::chrono;

TEST(TimeStamp_unittest, addition_operator)
{
    auto now = TimeStamp::now();
    auto nowInt = now.toMilliSec();
    auto delay = now + 1000; // add 1000ms to a TimeStamp
    auto delayInt = delay.toMilliSec();
    EXPECT_EQ(nowInt + 1000, delayInt);
}

TEST(TimeStamp_unittest, relational_operator)
{
    auto now = TimeStamp::now();
    auto delay = now + 1000; // add 1000us to a TimeStamp
    EXPECT_TRUE(now + 0 == now);
    EXPECT_TRUE(now == now);
    EXPECT_TRUE(now < delay);
    EXPECT_TRUE(now <= delay);
}
