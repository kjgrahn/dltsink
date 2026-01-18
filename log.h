/* Copyright (c) 2026 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef DLTSINK_LOG_H
#define DLTSINK_LOG_H

#include <string>
#include <fstream>

#include "message.h"
#include "timeval.h"

/**
 * A text log similar to syslog, either written to file or
 * (if an empty file name is given) to stdout.
 */
struct Log {
    template <class Arg>
    explicit Log(const Arg& arg);

    void connect(const timeval&);
    void disconnect(const timeval&);
    void log(const timeval&, const dlt::msg::Log&);

    bool broken = false;

private:
    const bool colorize;
    const bool flush;
    std::ofstream of;

    std::ostream& os();
    void end();
};

template <class Arg>
Log::Log(const Arg& arg)
    : colorize {arg.colorize},
      flush {arg.flush}
{
    if (arg.filename.size()) {
	of.open(arg.filename);
	broken = of.fail();
    }
}

#endif
