/*
 * Copyright (c) 2026 Jörgen Grahn
 * All rights reserved.
 *
 */
#include <string>
#include <iostream>
#include <cstring>
#include <getopt.h>

#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "socket.h"
#include "rxstream.h"
#include "message.h"
#include "log.h"

namespace {

    /* getaddrinfo() hints suitable for this, a TCP client.
     */
    addrinfo tcp_client()
    {
	addrinfo a = {};
	a.ai_flags = AI_ADDRCONFIG;
	a.ai_family = AF_UNSPEC;
	a.ai_socktype = SOCK_STREAM;
	return a;
    }

    timeval now()
    {
	timeval tv;
	(void)gettimeofday(&tv, nullptr);
	return tv;
    }

    timeval tv_of(double seconds)
    {
	time_t s = seconds;
	return {s, suseconds_t((seconds-s)*1e6) };
    }

    void sleep(std::chrono::microseconds dt)
    {
	/* might be interrupted, but never mind */
	(void)usleep(dt.count());
    }

    /**
     * Set the socket(7) SO_RCVTIMEO and SO_SNDTIMEO.
     */
    void set_timeout(int fd, double seconds)
    {
	const timeval tv = tv_of(seconds);
	setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof tv);
    }

    /* From one or more struct addrinfo, try to socket() and
     * connect(). Returns the fd, or sets errno and returns -1.
     *
     * We want certain properties for this socket:
     *
     * - We want to become connected as soon as possible, i.e. as soon
     *   as the host is reachable and the server is listening. We
     *   mustn't e.g. be stuck in slow SYN retransmission when this
     *   happens.
     *
     * - We must detect a broken connection as soon as possible, so we
     *   can start to reconnect.
     */
    int connect_one(const addrinfo* ais, double timeout)
    {
	for (const addrinfo* p=ais; p; p=p->ai_next) {
	    int fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
	    if (fd==-1) continue;

	    set_timeout(fd, timeout);

	    int err = connect(fd, p->ai_addr, p->ai_addrlen);
	    if (!err) return fd;
	    close(fd);
	}
	return -1;
    }

    size_t size(const std::vector<Range>& v)
    {
	size_t n = 0;
	for (auto& e: v) n += e.size();
	return n;
    }

    void dltsink(Log&, const addrinfo*);

    /* Try forever to connect and reconnect to the dlt daemon, and
     * while connected, log everything.
     */
    template <class Arg>
    int dltsink(std::ostream& err, const Arg& arg)
    {
	const addrinfo hints = tcp_client();
	addrinfo* ais;
	const int rc = getaddrinfo(arg.host.c_str(), arg.port.c_str(),
				   &hints, &ais);
	if (rc) {
	    err << "error: '" << arg.host << "': " << gai_strerror(rc) << '\n';
	    return 1;
	}

	Log log {arg};
	if (log.broken) {
	    err << "error: failed to open " << arg.filename
		<< " for writing: " << std::strerror(errno) << '\n';
	    return 1;
	}

	while (true) {
	    dltsink(log, ais);
	}

	freeaddrinfo(ais);
	return 0;
    }

    void dltsink(Log& log, const addrinfo* ais)
    {
	const timeval t0 = now();
	const Socket fd {connect_one(ais, 3.14)};
	timeval t = now();

	if(fd.invalid()) {
	    log.connect_fail(t, fd.error());
	    if (t - t0 < std::chrono::seconds{1}) {
		/* The connect failed quickly: wait a bit before
		 * returning, because returning implies retrying.
		 */
		sleep(std::chrono::seconds{1});
	    }
	    return;
	}

	log.connect(t);
	RxStream rx;

	while (true) {
	    const ssize_t rc = feed(rx, fd);
	    t = now();
	    if (rc < 0) break;
	    if (rc == 0) break;

	    const auto v = rx.messages();
	    for (const Range& r : v) {
		const dlt::msg::Log msg {r};
		if (!msg.valid()) continue;
		log.log(t, msg);
	    }
	    rx.consume(size(v));
	}
	log.disconnect(t);

	if (t - t0 < std::chrono::seconds{1}) {
	    /* Very short session: wait a bit before returning,
	     * because the next one might become just as short.
	     */
	    sleep(std::chrono::seconds{2});
	}
    }
}

int main(int argc, char ** argv)
{
    const std::string prog = argv[0];
    const std::string usage = "usage: "
	+ prog + " [-c] [-e] [-b] [-p port] [-o file] host\n"
	"       "
	+ prog + " --help\n"
	"       "
	+ prog + " --version";
    const char optstring[] = "cebp:o:";
    const struct option long_options[] = {
	{"version", 0, 0, 'V'},
	{"help", 0, 0, 'H'},
	{0, 0, 0, 0}
    };

    struct {
	bool colorize = false;
	bool ecu = false;
	bool flush = true;
	std::string host;
	std::string port = "3490";
	std::string filename;
    } arg;

    int ch;
    while((ch = getopt_long(argc, argv,
			    optstring,
			    &long_options[0], 0)) != -1) {
	switch(ch) {
	case 'c':
	    arg.colorize = true;
	    break;
	case 'e':
	    arg.ecu = true;
	    break;
	case 'b':
	    arg.flush = false;
	    break;
	case 'p':
	    arg.port = optarg;
	    break;
	case 'o':
	    arg.filename = optarg;
	    break;
	case 'V':
	    std::cout << prog << " 1.0\n"
		      << "Copyright (c) 2026 Jörgen Grahn\n";
	    return 0;
	    break;
	case 'H':
	    std::cout << usage << '\n';
	    return 0;
	    break;
	case ':':
	case '?':
	    std::cerr << usage << '\n';
	    return 1;
	    break;
	default:
	    break;
	}
    }

    if(optind+1 != argc) {
	std::cerr << usage << '\n';
	return 1;
    }

    arg.host = argv[optind];

    return dltsink(std::cerr, arg);
}
