/* Copyright (c) 2026 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "message.h"

#include "be.h"

using dlt::msg::Log;

namespace {

    bool unusual(unsigned htyp)
    {
	return htyp != 0x3d;
    }

    bool not_log(unsigned msin)
    {
	return msin & 0x0e;
    }

    template <size_t N>
    std::array<char, N> get(const uint8_t* p)
    {
	std::array<char, N> res;
	for (char& c : res) c = *p++;
	return res;
    }

    bool isspace(unsigned ch)
    {
	return std::isspace(ch);
    }

    /* Make s not end with whitespace.
     */
    void trimr(std::string& s)
    {
	while (s.size() && isspace(s.back())) s.pop_back();
    }
}

/* Create from a range which is only known to have a valid outer
 * length. Example:
 *
 * htyp  3d
 * mcnt  0c
 * len   00 58
 * ecu   45 43 55 31
 *       00 05 25 14
 * ts    6d ec 36 34
 * msin  41
 * noar  01
 * app   44 4c 54 44
 * ctx   49 4e 54 4d
 *       00 02 00
 * len   00 38
 *       00 4e 65 77 20 63 6c 69 65 6e 74 20 63 6f 6e 6e
 *       65 63 74 69 6f 6e 20 23 39 20 65 73 74 61 62 6c
 *       69 73 68 65 64 2c 20 54 6f 74 61 6c 20 43 6c 69
 *       65 6e 74 73 20 3a 20 32
 *       00
 */
Log::Log(Range r)
    : ts{}
{
    auto a = r.begin();
    const unsigned htyp = *a++;
    if (unusual(htyp)) return;
    a += 3;
    ecu = get<4>(a); a += 4;
    a += 4;
    const unsigned msin = a[4];
    if (not_log(msin)) return;
    level = LogLevel { msin >> 4};
    ts = Duration {be::get32(a)}; a += 4;
    a += 2;
    app = get<4>(a); a += 4;
    ctx = get<4>(a); a += 4;

    // so I guess the rest is some junk around the string
    auto b = r.end();
    a += 6;
    b--;
    text = {a, b};
    trimr(text);
}
