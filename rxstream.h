/* Copyright (c) 2026 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef DLTSINK_RXSTREAM_H
#define DLTSINK_RXSTREAM_H

#include "range.h"

#include <cstdint>
#include <vector>
#include <unistd.h>

class Socket;

/**
 * A typical TCP Rx stream, for the DLT "protocol":
 * - feeding the stream from a socket
 * - finding 0 or more complete messages in the stream,
 *   or discovering that the stream is corrupted
 * - removing those messages from the stream after use
 */
class RxStream {
public:
    void feed(Range);
    std::vector<Range> messages() const;
    void consume(size_t);

private:
    std::vector<uint8_t> v;
};

ssize_t feed(RxStream&, const Socket&);

#endif
