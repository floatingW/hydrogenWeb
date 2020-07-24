/*
 * File: Timer.cpp
 * ---------------
 * @author: Fu Wei
 *
 */

#include "core/Timer.hpp"

void Timer::restart(Timestamp now)
{
    if (_repeat)
    {
        _endTime = now + _interval;
    }
}
