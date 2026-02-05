/* Copyright (c) 2026 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "timestamp.h"

#include <cctype>
#include <cstdlib>

namespace {
    bool digit(unsigned ch)
    {
	return std::isdigit(ch);
    }

    bool is_milli(unsigned long long n) { return n > 950e9; }
    bool is_seconds(unsigned long long n) { return n > 950e6; }
}

/* Find a 21st century timestamp in a string, either as seconds since
 * epoch, or milliseconds since epoch. Or return a zero time if there
 * is no timestamp.
 *
 * For example, "1770290166" is a time in February, 2026, and so is
 * "1770290166098", except in milliseconds. "3170290166" is a time in
 * June, 2070.
 *
 * Returned as a duration since epoch rather than a time_point to
 * avoid involving clocks.
 */
std::chrono::milliseconds system_time(const std::string& s)
{
    const char* p = s.c_str();

    while (*p) {
	if (!digit(*p)) {
	    p++;
	    continue;
	}
	char* end;
	const auto n = std::strtoull(p, &end, 10);
	if (is_milli(n)) return std::chrono::milliseconds {n};
	if (is_seconds(n)) return std::chrono::seconds {n};
	p = end;
    }

    return {};
}
