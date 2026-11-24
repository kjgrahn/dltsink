/* Copyright (c) 2026 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "message.h"

#include "be.h"

namespace {

    bool unusual(unsigned htyp)
    {
	return htyp != 0x3d;
    }

    bool not_log(unsigned msin)
    {
	return msin & 0x0e;
    }

    bool isprint(unsigned ch)
    {
	return std::isprint(ch);
    }

    template <size_t N>
    std::array<char, N> get(const uint8_t* p)
    {
	std::array<char, N> res;
	for (char& c : res) {
	    c = *p++;
	    if (!isprint(c)) c = '-';
	}
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
dlt::msg::v1::Log::Log(Range r)
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

/* As above, but for DLT v2. Example:
 *
 * htyp  5c 00 00 00
 * mcnt  a9
 * len   00 9a
 * msin  21
 * noar  01
 * ns    e0 99 e2 11      endianness bug: COVESA/dlt-daemon e3bc133
 * ts    00 6b 04 10 f8
 * ecu   04 aa aa aa aa
 * app   04 bb bb bb bb
 * ctx   04 cc cc cc cc
 *       00 00 02 fb
 *       00 02 00
 * len   00 6f
 *       00
 *       text ...
 *       00
 *
 */
dlt::msg::v2::Log::Log(Range r)
    : tv{}
{
    auto a = r.begin();
    const unsigned htyp = *a++;
    if (htyp >> 5 != 2) return;
    a += 6;
    const unsigned msin = *a++;
    if (not_log(msin)) return;
    level = LogLevel { msin >> 4};
    a += 1;

    tv.tv_usec = le::get32(a) / 1000; a += 4;
    tv.tv_sec  = be::get40(a); a += 5;

    ecu = get<4>(++a); a += 4;
    app = get<4>(++a); a += 4;
    ctx = get<4>(++a); a += 4;

    auto b = r.end();
    a += 10;
    b--;
    text = {a, b};
    trimr(text);
}
