/*
 * File: Timestamp.cpp
 * -------------------
 * @author: Fu Wei
 *
 */

#include "core/Timestamp.hpp"

#include <cstdio> // snprintf()
#include <cinttypes> // for printing format specifiers' macros
#include <algorithm> // fill_n()

bool operator<(const Timestamp& lhs, const Timestamp& rhs)
{
    using namespace std::chrono;
    auto lus = duration_cast<microseconds>(lhs._timePoint.time_since_epoch()).count();
    auto rus = duration_cast<microseconds>(rhs._timePoint.time_since_epoch()).count();
    return lus < rus;
}

bool operator<=(const Timestamp& lhs, const Timestamp& rhs)
{
    using namespace std::chrono;
    auto lus = duration_cast<microseconds>(lhs._timePoint.time_since_epoch()).count();
    auto rus = duration_cast<microseconds>(rhs._timePoint.time_since_epoch()).count();
    return lus <= rus;
}

bool operator==(const Timestamp& lhs, const Timestamp& rhs)
{
    using namespace std::chrono;
    auto lus = duration_cast<microseconds>(lhs._timePoint.time_since_epoch()).count();
    auto rus = duration_cast<microseconds>(rhs._timePoint.time_since_epoch()).count();
    return lus == rus;
}

Timestamp operator+(const Timestamp& lhs, double sec)
{
    using namespace std::chrono;
    auto microSecs = static_cast<int64_t>(sec * Timestamp::microSecsPerSecond);
    return Timestamp(lhs._timePoint + microseconds(microSecs));
}

std::string Timestamp::toString()
{
    auto microSec = toMicroSec();
    auto seconds = microSec / microSecsPerSecond;
    auto micros = microSec % microSecsPerSecond;
    char buf[64];
    std::fill_n(buf, 64, 0);

    snprintf(buf, sizeof(buf) - 1, "%" PRId64 ".%06" PRId64 "", seconds, micros);

    return buf;
}
