/* Copyright (c) 2026 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "log.h"

#include <iostream>

namespace {

    const char SP = ' ';
    const char SP11[] = "           ";
}

void Log::connect(const timeval& tv)
{
    const Color& c = colors.blue;
    os() << tv << SP11
	 << c << "connected" << c.reset << '\n';
    end();
}

void Log::connect_fail(const timeval& tv, const std::string& err)
{
    const Color& c = colors.blue;
    os() << tv << SP11
	 << c << "connect failed: " << err << c.reset << '\n';
    end();
}

void  Log::disconnect(const timeval& tv)
{
    const Color& c = colors.blue;
    os() << tv << SP11
	 << c << "disconnected" << c.reset << '\n';
    end();
}

namespace {

    const char* str(dlt::msg::LogLevel val)
    {
	switch (val.val) {
	case 1:  return "fatal";
	case 2:  return "error";
	case 3:  return "warn ";
	case 4:  return "info ";
	case 5:  return "debug";
	case 6:  return "verb ";
	default: return "???? ";
	}
    }

    template <class Colors>
    const Color& select(dlt::msg::LogLevel val,
			const Colors& cc)
    {
	switch (val.val) {
	case 1:
	case 2:
	case 3:  return cc.yellow;
	case 5:
	case 6:  return cc.green;
	default: return cc.normal;
	}
    }
}

void Log::log(const timeval& tv, const dlt::msg::Log& msg)
{
    const Color& c = select(msg.level, colors);
    msg.put(os(), tv, c);
    end();
}

std::ostream& Log::os()
{
    if (of.is_open()) return of;
    return std::cout;
}

void Log::end()
{
    if (flush) os().flush();
}

std::ostream& operator<< (std::ostream& os, const dlt::msg::Tag& val)
{
    return os.write(val.data(),
		    val.size());
}

std::ostream& operator<< (std::ostream& os, const dlt::msg::LogLevel& val)
{
    return os << str(val);
}

std::ostream& dlt::msg::Log::put(std::ostream& os,
				 const timeval& tv, const Color& c) const
{
    os << tv
       << SP << app
       << SP << ctx
       << SP << c << level << c.reset
       << SP << text
       << '\n';
    return os;
}
