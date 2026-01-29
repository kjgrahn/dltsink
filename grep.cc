/* Copyright (c) 2026 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "grep.h"

#include <algorithm>


void Grep::add(const std::string& s)
{
    dlt::msg::Tag tag;
    if (s.size() != tag.size()) return;
    std::copy(begin(s), end(s), begin(tag));
    v.push_back(tag);
}

bool Grep::match(dlt::msg::Tag val) const
{
    if (v.empty()) return true;

    // linear search beats fancy algorithms for real-world data sets
    auto it = std::find(begin(v), end(v), val);
    return it != end(v);
}
