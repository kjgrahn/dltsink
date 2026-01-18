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
}

#endif
