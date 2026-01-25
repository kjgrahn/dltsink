/* Copyright (c) 2026 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "color.h"

#include <iostream>

Color::Color(bool color, const char* s)
    : s { color ? s : nullptr },
      reset {color? "\033[0m" : ""}
{}

std::ostream& operator<< (std::ostream& os, const Color& val)
{
    if (!val.s) return os;
    return os << "\033[" << val.s;
}
