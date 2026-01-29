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
#include "color.h"

/**
 * A text log similar to syslog, either written to file or
 * (if an empty file name is given) to stdout.
 */
struct Log {
    template <class Arg>
    explicit Log(const Arg& arg);

    void connect(const timeval&);
    void connect_fail(const timeval&, const std::string& err);
    void disconnect(const timeval&);
    void log(const timeval&, const dlt::msg::Log&);

    bool broken = false;

private:
    const struct {
	Color yellow;
	Color green;
	Color blue;
	Color normal;
    } colors;
    const bool flush;
    const struct {
	bool ctx;
	bool ecu;
    } with;
    std::ofstream of;

    std::ostream& os();
    void end();
};

template <class Arg>
Log::Log(const Arg& arg)
    : colors { Color {arg.colorize, "0;33m"},
	       Color {arg.colorize, "0;32m"},
	       Color {arg.colorize, "0;36m"},
	       Color {false, nullptr} },
      flush {arg.flush},
      with {arg.ctx, arg.ecu}
{
    if (arg.filename.size()) {
	of.open(arg.filename);
	broken = of.fail();
    }
}

#endif
