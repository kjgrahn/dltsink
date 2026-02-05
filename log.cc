/* Copyright (c) 2026 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "log.h"
#include "timestamp.h"

#include <iostream>

namespace {

    const char SP = ' ';
    const char SP2[] = "  ";
}

void Log::connect(const timeval& tv)
{
    const Color& c = colors.blue;
    os() << tv << SP2 << c << "connected" << c.reset << '\n';
    end();
}

void Log::connect_fail(const timeval& tv, const std::string& err)
{
    const Color& c = colors.blue;
    os() << tv << SP2 << c << "connect failed: " << err << c.reset << '\n';
    end();
}

void  Log::disconnect(const timeval& tv)
{
    epoch.reset();
    const Color& c = colors.blue;
    os() << tv << SP2 << c << "disconnected" << c.reset << '\n';
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
	case 3:  return cc.red;
	case 4:  return cc.normal;
	case 5:
	case 6:  return cc.green;
	default: return cc.normal;
	}
    }

    /* Try to find a system time in a message, so we can synchronize
     * the clocks. Typically returns zero time, meaning we didn't find
     * any.
     */
    dlt::Epoch::Ms system_time(const dlt::msg::Tag app, const dlt::msg::Log& msg)
    {
	if (msg.app != app) return {};
	return ::system_time(msg.text);
    }

    /* The system timestamp to use for msg: either based on msg.ts and
     * the Epoch, or else the local processing timestamp.
     */
    timeval time_of(const dlt::msg::Log& msg, std::unique_ptr<dlt::Epoch>& epoch, timeval local)
    {
	if (!epoch) return local;
	auto ms = epoch->translate(msg.ts);
	return to_timeval(ms);
    }
}

void Log::log(const timeval& tv, const dlt::msg::Log& msg)
{
    auto t = system_time(timesrc, msg);
    if (t.count()) {
	epoch = std::make_unique<dlt::Epoch>(msg.ts, t);
    }

    if (!grep.match(msg.app)) return;

    const Color& c = select(msg.level, colors);
    msg.put(os(), time_of(msg, epoch, tv),
	    with.ctx, with.ecu, c);
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

std::ostream& dlt::msg::Log::put(std::ostream& os, const timeval& tv,
				 bool with_ctx, bool with_ecu,
				 const Color& c) const
{
    os << tv << SP2;
    if (with_ecu) os << ecu << SP;
    os << app << SP;
    if (with_ctx) os << ctx << SP;
    os << c << level << c.reset << SP
       << text
       << '\n';
    return os;
}
