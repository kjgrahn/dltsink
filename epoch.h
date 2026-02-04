/* Copyright (c) 2026 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef DLTSINK_EPOCH_H
#define DLTSINK_EPOCH_H

#include "message.h"
#include <chrono>

namespace dlt {

    /**
     * Estimating system time from time in a variable-epoch clock.
     *
     * It's like this:
     *
     * - DLT messages are timestamped, unclear whether at
     *   the source or in the daemon.
     *
     * - The timestamps are 4 bytes, with resolution 100us, and
     *   relative to system start. It wraps around in 5 days.
     *
     * - A source which is aware of real system time can log it, thereby
     *   synchronizing the clocks.
     *
     * - A sink which tracks the stream can look for synchronization
     *   messages, and use the latest one to translate a DLT timestamp
     *   to true system time.
     *
     *   This cannot be done until a synchronization message
     *   arrives. It also assumes log messages show up more
     *   frequently than the DLT clock wraps around, so that
     *   wraparound can be detected and dealt with.
     */
    class Epoch {
    public:
	static constexpr auto period() {
	    return std::chrono::duration<unsigned long long,
					 msg::Duration::period> { 1ull << 32 };
	};

	using Ms = std::chrono::milliseconds;

	Epoch(msg::Duration t0, Ms t1);

	Ms translate(msg::Duration t);

    private:
	msg::Duration t0;
	msg::Duration prev;
	unsigned wrap = 0;
	Ms t1;
    };

}
#endif
