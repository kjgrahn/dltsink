/*
 * Copyright (c) 2026 Jörgen Grahn
 * All rights reserved.
 *
 */
#include <string>
#include <iostream>
#include <cstring>
#include <getopt.h>

namespace {

}

int main(int argc, char ** argv)
{
    const std::string prog = argv[0];
    const std::string usage = "usage: "
	+ prog + " [-p port] [-o file] host\n"
	"       "
	+ prog + " --help\n"
	"       "
	+ prog + " --version";
    const char optstring[] = "p:o:";
    const struct option long_options[] = {
	{"version", 0, 0, 'V'},
	{"help", 0, 0, 'H'},
	{0, 0, 0, 0}
    };

    std::cin.sync_with_stdio(false);
    std::cout.sync_with_stdio(false);

    struct {
	std::string host;
	std::string port = "3490";
	std::string file;
    } arg;

    int ch;
    while((ch = getopt_long(argc, argv,
			    optstring,
			    &long_options[0], 0)) != -1) {
	switch(ch) {
	case 'p':
	    arg.port = optarg;
	    break;
	case 's':
	    arg.file = optarg;
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

    return 0;
}
