/*
 * Copyright (c) 2016 Jörgen Grahn
 * All rights reserved.
 *
 */
#include <timeval.h>

#include <orchis.h>
#include <sstream>
#include <ctime>
#include <cassert>


namespace {

    time_t now()
    {
	struct tm n { 59, 16, 13,
		      24, 8, 116,
		      0, 0, -1 };
	return std::mktime(&n);
    }

    std::string str(const struct timeval& val)
    {
	std::ostringstream oss;
	oss << val;
	return oss.str();
    }
}


namespace tv {

    void simple(orchis::TC)
    {
	orchis::assert_eq(str({now(), 0u}),
			  "13:16:59.000");
    }

    void rounding(orchis::TC)
    {
	orchis::assert_eq(str({now(), 0u}),
			  "13:16:59.000");
	orchis::assert_eq(str({now(), 1400u}),
			  "13:16:59.001");
	orchis::assert_eq(str({now(), 1600u}),
			  "13:16:59.002");
	orchis::assert_eq(str({now(), 999900u}),
			  "13:17:00.000");
    }

    void chrono_conversion(orchis::TC)
    {
	std::chrono::milliseconds ms {123456};
	const timeval tv = to_timeval(ms);

	// assumes TZ=C
	orchis::assert_eq(str(tv), "00:02:03.456");
    }
}
