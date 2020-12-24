#include "quotes/session.h"

#include <boost/asio/error.hpp>
#include <iostream>
#include <istream>

#include <boost/asio/buffer.hpp>
#include <boost/asio/completion_condition.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind/bind.hpp>

#include "quotes.pb.h"

namespace quotes {

using boost::system::error_code;

Session::Session(Service ioService) noexcept : mSocket{*ioService} {
    std::cout << "New session" << std::endl;
}

Session::pointer Session::create(Service ioService) noexcept {
    return pointer{new Session(std::move(ioService))};
}

void Session::startReading() noexcept {
    auto bind = boost::bind(
        &Session::handleRead, shared_from_this(),
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred);

    boost::asio::async_read(
        mSocket, mReadBuffer, boost::asio::transfer_at_least(1), bind);
}

void Session::write(const proto::Packet & packet) noexcept {
    auto data = packet.SerializeAsString();
    auto bind = boost::bind(
        &Session::handleWrite, shared_from_this(),
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred);

    boost::asio::async_write(mSocket, boost::asio::buffer(data), bind);
}

void Session::handleRead(error_code code, size_t bytes) noexcept {
    if (!code) {
        proto::Packet packet;
        std::istream stream{&mReadBuffer};
        auto parsed = packet.ParseFromIstream(&stream);
        // if (parsed)
    } else if (
        code == boost::asio::error::eof ||
        code == boost::asio::error::connection_reset) {
        std::cout << "Disconnected" << std::endl;
    }
}

void Session::handleWrite(error_code code, size_t bytes) noexcept {}

tcp::socket & Session::socket() noexcept {
    return mSocket;
}

}  // namespace quotes
