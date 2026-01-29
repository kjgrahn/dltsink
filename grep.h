/* Copyright (c) 2026 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef DLTSINK_GREP_H
#define DLTSINK_GREP_H

#include "message.h"

#include <string>
#include <vector>

/**
 * Like fgrep -e foo -e bar ... That is, filtering a log message
 * for one of several fixed strings, or not filtering at all.
 */
class Grep {
public:
    void add(const std::string& s);
    bool match(dlt::msg::Tag) const;

private:
    std::vector<dlt::msg::Tag> v;
};

#endif
