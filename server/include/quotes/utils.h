#pragma once

#include <iostream>
#include <mutex>
#include <string>

namespace quotes {

template <class... Args>
void log(Args &&... args) noexcept {
    static std::mutex sMutex;
    std::lock_guard lock{sMutex};
    ((std::cout << args << " "), ...);
    std::cout << std::endl;
}

std::string timeToString(uint64_t microseconds) noexcept;

}  // namespace quotes
