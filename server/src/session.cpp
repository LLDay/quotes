#include "quotes/session.h"

#include <istream>

#include <boost/asio/bind_executor.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/completion_condition.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind/bind.hpp>

#include "quotes.pb.h"
#include "quotes/server.h"
#include "quotes/types.h"

namespace quotes {

using boost::system::error_code;

Session::Session(Service ioService, EventPointer event) noexcept
    : mSocket{*ioService}, mStrand{*ioService}, mEvents{std::move(event)} {}

SessionPointer Session::create(Service ioService, EventPointer event) noexcept {
    return SessionPointer{new Session{std::move(ioService), std::move(event)}};
}

void Session::startReading() noexcept {
    auto bindObject = boost::asio::bind_executor(
        mStrand, boost::bind(
                     &Session::handleRead, shared_from_this(),
                     boost::asio::placeholders::error,
                     boost::asio::placeholders::bytes_transferred));

    boost::asio::async_read(
        mSocket, mReadBuffer, boost::asio::transfer_at_least(1), bindObject);
}

void writeHandler() noexcept {}

void Session::send(const proto::Packet & packet) noexcept {
    auto data = packet.SerializeAsString();
    boost::asio::async_write(
        mSocket, boost::asio::buffer(data), boost::bind(&writeHandler));
}

void Session::handleRead(error_code error, size_t bytes) noexcept {
    if (error == boost::asio::error::eof ||
        error == boost::asio::error::connection_reset) {
        if (!mEvents.expired()) {
            auto events = mEvents.lock();
            events->onSessionDisconnected(shared_from_this());
        }
        return;
    } else if (!error) {
        proto::Packet packet;
        std::istream stream{&mReadBuffer};
        auto parsed = packet.ParseFromIstream(&stream);
        if (parsed && !mEvents.expired()) {
            auto events = mEvents.lock();
            events->onPacketRead(shared_from_this(), packet);
        }
    }
    startReading();
}

tcp::socket & Session::socket() noexcept {
    return mSocket;
}

void Session::close() noexcept {
    mSocket.shutdown(tcp::socket::shutdown_receive);
    mSocket.close();
}

}  // namespace quotes
