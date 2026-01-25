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
    os() << tv << SP11 << "connected\n";
    end();
}

void Log::connect_fail(const timeval& tv, const std::string& err)
{
    os() << tv << SP11 << "connect failed: " << err << '\n';
    end();
}

void  Log::disconnect(const timeval& tv)
{
    os() << tv << SP11 << "disconnected\n";
    end();
}

void Log::log(const timeval& tv, const dlt::msg::Log& msg)
{
    msg.put(os(), tv);
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

std::ostream& dlt::msg::Log::put(std::ostream& os, const timeval& tv) const
{
    os << tv
       << SP << app
       << SP << ctx
       << SP << level
       << SP << text
       << '\n';
    return os;
}
