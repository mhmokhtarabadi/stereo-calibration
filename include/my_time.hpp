#pragma once

#include <iostream>
#include <chrono>

std::chrono::_V2::system_clock::time_point getTimeNow();

int64_t msPassedSince(std::chrono::_V2::system_clock::time_point previousPoint);
