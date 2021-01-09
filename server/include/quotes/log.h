#pragma once

#include <iostream>
#include <mutex>

namespace quotes {

template <class... Args>
void log(Args &&... args) noexcept {
    static std::mutex sMutex;
    std::lock_guard lock{sMutex};
    ((std::cout << args << " "), ...);
    std::cout << std::endl;
}

}  // namespace quotes
