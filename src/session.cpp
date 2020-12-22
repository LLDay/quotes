#include "quotes/session.h"

namespace quotes {

Session::Session(Service ioService) noexcept : mSocket{*ioService} {}

Session::pointer Session::create(Service ioService) noexcept {
    return pointer{new Session(std::move(ioService))};
}

void Session::handleRead() noexcept {}

void Session::handleWrite() noexcept {}

tcp::socket & Session::socket() noexcept {
    return mSocket;
}

}  // namespace quotes
