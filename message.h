/* Copyright (c) 2026 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef DLTSINK_MESSAGE_H
#define DLTSINK_MESSAGE_H

#include "range.h"
#include "color.h"

#include <array>
#include <string>
#include <chrono>

#include <iosfwd>

class timeval;

namespace dlt {
    namespace msg {

	using Tag = std::array<char, 4>;
	using Duration = std::chrono::duration<unsigned, std::ratio<1, 10000>>;

	struct LogLevel { unsigned val; };

	/**
	 * A DLT log message: text and various metadata. Might not be valid(),
	 * in which case it was probably some other kind of message.
	 *
	 */
	class Log {
	public:
	    explicit Log(Range);
	    bool valid() const { return ts.count(); }

	    std::ostream& put(std::ostream&, const timeval&, const Color&) const;

	    Tag ecu;
	    Duration ts;
	    Tag app;
	    Tag ctx;
	    LogLevel level;
	    std::string text;
	};
    }
}

#endif
