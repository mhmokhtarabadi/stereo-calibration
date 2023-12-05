
#include "my_time.hpp"

std::chrono::_V2::system_clock::time_point getTimeNow()
{
    return std::chrono::high_resolution_clock::now();
}

int64_t msPassedSince(std::chrono::_V2::system_clock::time_point previousPoint)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - previousPoint).count();
}