/* Copyright (c) 2026 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef DLTSINK_BE_H
#define DLTSINK_BE_H

#include <cstdint>

namespace be {

    inline
    unsigned get8(const uint8_t* p)
    {
	return *p++;
    }

    inline
    unsigned get16(const uint8_t* p)
    {
	unsigned n = get8(p++) << 8;
	n |= get8(p++);
	return n;
    }

    inline
    unsigned get32(const uint8_t* p)
    {
	unsigned n = get16(p) << 16;
	p += 2;
	n |= get16(p);
	return n;
    }

    inline
    unsigned long get40(const uint8_t* p)
    {
	unsigned long n = get8(p++);
	n = n << 32;
	n |= get32(p);
	return n;
    }
}

namespace le {

    inline
    unsigned get16(const uint8_t* p)
    {
	unsigned n = *p++;
	n |= *p << 8;
	return n;
    }

    inline
    unsigned get32(const uint8_t* p)
    {
	unsigned n = get16(p);
	p += 2;
	n |= get16(p) << 16;
	return n;
    }
}

#endif
