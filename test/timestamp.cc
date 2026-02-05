/* Copyright (c) 2026 Jörgen Grahn
 * All rights reserved.
 *
 */
#include <timestamp.h>

#include <orchis.h>

namespace timestamp {

    using orchis::TC;
    using orchis::assert_eq;

    void assert_no_match(const char* s)
    {
	auto t = system_time(s);
	assert_eq(t, t.zero());
    }

    void assert_match(const char* s)
    {
	using sec = std::chrono::seconds;
	const std::vector<std::chrono::milliseconds> v {
	    sec { 950292000}, //  2000
	    sec {1070292003}, //  2003
	    sec {1370292013}, //  2013
	    sec {1770292026}, //  2026
	    sec {3170292070}, //  2070
	};
	auto t = system_time(s);
	orchis::assert_in(v, t);
    }

    namespace seconds {

	void simple(TC)
	{
	    assert_match( "950292000");
	    assert_match("1070292003");
	    assert_match("1770292026");
	    assert_match("3170292070");
	}

	void empty(TC)
	{
	    assert_no_match("");
	}

	void nothing(TC)
	{
	    assert_no_match(" foo bar baz");
	}

	void something(TC)
	{
	    assert_no_match(" 14 foo bar 17702920 13 2 baz");
	}

	void chaff(TC)
	{
	    assert_match(" 14 foo bar 950292000 13 2 baz");
	    assert_match(" 14 foo bar 1770292026 13 2 baz");
	}
    }

    namespace milli {

	void simple(TC)
	{
	    assert_match( "950292000000");
	    assert_match("1070292003000");
	    assert_match("1770292026000");
	    assert_match("3170292070000");
	}

	void chaff(TC)
	{
	    assert_match(" 14 foo bar 950292000000 13 2 baz");
	    assert_match(" 14 foo bar 1770292026000 13 2 baz");
	}
    }
}
