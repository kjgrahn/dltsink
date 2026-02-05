/* Copyright (c) 2026 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef DLTSINK_TIMESTAMP_H
#define DLTSINK_TIMESTAMP_H

#include <chrono>
#include <string>

std::chrono::milliseconds system_time(const std::string& msg);

#endif
