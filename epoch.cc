/* Copyright (c) 2026 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "epoch.h"

using dlt::Epoch;

/* Synchronization: a log message stamped with t0 mentions
 * system time t1.
 */
Epoch::Epoch(msg::Duration t0, Ms t1)
    : t0 {t0},
      prev {t0},
      t1 {t1}
{}

/* Translate 't' to system time. Note that call order matters.
 * translate(t); translate(t-N) is assumed to mean the timestamp
 * has wrapped around once.
 */
Epoch::Ms Epoch::translate(msg::Duration t)
{
    if (t < prev) {
	wrap++;
    }
    prev = t;
    return std::chrono::duration_cast<Ms>(t1 + t - t0 + wrap * period());
}
