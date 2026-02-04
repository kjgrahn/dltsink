/*
 * Copyright (c) 2026 Jörgen Grahn
 * All rights reserved.
 *
 */
#include <epoch.h>

#include <orchis.h>

namespace epoch {

    using orchis::TC;
    using orchis::assert_eq;

    void period(TC)
    {
	const dlt::msg::Duration t0 {};
	const dlt::msg::Duration t1 = t0 + dlt::Epoch::period();
	assert_eq(t0, t1);
    }

    void simple(TC)
    {
	const auto t0 = dlt::msg::Duration {5 * 10000};
	const auto t1 = std::chrono::seconds {1770236414};

	dlt::Epoch epoch {t0, t1};
	assert_eq(epoch.translate(t0), t1);

	const auto dt = std::chrono::hours {1};
	assert_eq(epoch.translate(t0 + dt), t1 + dt);
    }

    void wraparound(TC)
    {
	const auto tb = dlt::msg::Duration {0xb0u << 24};
	const auto td = dlt::msg::Duration {0xd0u << 24};
	const auto tf = dlt::msg::Duration {0xf0u << 24};

	const auto t1 = std::chrono::seconds {1770236414};

	auto trunc = [] (auto dt) {
	    return std::chrono::duration_cast<std::chrono::milliseconds>(dt);
	};

	dlt::Epoch epoch {tb, t1};
	assert_eq(epoch.translate(tb), t1);
	assert_eq(epoch.translate(td), trunc(t1 + td - tb));
	assert_eq(epoch.translate(tf), trunc(t1 + tf - tb));

	// td < tf, so time must have wrapped around
	assert_eq(epoch.translate(td), trunc(t1 + epoch.period() + td - tb));
	assert_eq(epoch.translate(td), trunc(t1 + epoch.period() + td - tb));
	assert_eq(epoch.translate(tf), trunc(t1 + epoch.period() + tf - tb));
    }
}
