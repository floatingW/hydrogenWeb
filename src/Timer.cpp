/*
 * File: Timer.cpp
 * ---------------------------------
 * @author: Fu Wei
 *
 */

#include "core/Timer.hpp"

void Timer::restart(TimeStamp now)
{
    if (_repeat)
    {
        _endTime = now + _interval;
    }
}
