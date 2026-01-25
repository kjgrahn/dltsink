/* Copyright (c) 2026 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef DLTSINK_COLOR_H
#define DLTSINK_COLOR_H

#include <iosfwd>

/**
 * An ANSI color: support for turning it on and off:
 *
 *   os << red << "foo" << red.reset;
 *
 * Or doing nothing if ANSI coloring is disabled.
 */
struct Color {
    explicit Color(bool color, const char* s);
    const char* const s;
    const char* const reset;
};

std::ostream& operator<< (std::ostream& os, const Color& val);

#endif
