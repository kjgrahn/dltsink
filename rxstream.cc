/* Copyright (c) 2026 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "rxstream.h"

#include "be.h"
#include "socket.h"

namespace {

    template <class Cont, class It>
    void append(Cont& c, It a, It b)
    {
	c.insert(std::end(c), a, b);
    }
}

void RxStream::feed(const Range r)
{
    append(v, r.begin(), r.end());
}

/* Find all complete DLT messages on the stream and return them as
 * Ranges. Throws if there can never be such a message. In that case,
 * you might lose some preceding messages, too.
 */
std::vector<Range> RxStream::messages() const
{
    std::vector<Range> acc;
    const uint8_t* a = v.data();
    const auto b = a + v.size();

    while (b-a > 4) {
	unsigned n = be::get16(a+2);
	if (n<4) throw tiff::Wedged {};
	if (a+n > b) break;
	acc.emplace_back(a, a+n);
	a += n;
    }
    return acc;
}

/* Pop n bytes from the stream. Preferably the n bytes constituting
 * the latest processed messages.
 */
void RxStream::consume(size_t n)
{
    auto a = v.begin();
    v.erase(a, a+n);
}

/* An ordinary read(2), but reads from a Socket and appends to the
 * Stream.
 */
ssize_t feed(RxStream& rx, const Socket& fd)
{
    uint8_t buf[8000];
    ssize_t rc = ::read(fd.fd, buf, sizeof buf);
    if (rc > 0) {
	size_t n = rc;
	rx.feed(Range{buf, buf + n});
    }
    return rc;
}
