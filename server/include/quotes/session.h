#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/system/error_code.hpp>

#include "quotes/events.h"
#include "quotes/types.h"

namespace quotes {

namespace proto {
class Packet;
}

using boost::asio::ip::tcp;

class Session : public boost::enable_shared_from_this<Session> {
 private:
    explicit Session(Service ioService, EventPointer events) noexcept;

 public:
    static SessionPointer create(
        Service ioService,
        EventPointer events) noexcept;

    void startReading() noexcept;

    void write(const proto::Packet & packet) noexcept;

    void handleRead(boost::system::error_code error, size_t bytes) noexcept;

    void handleWrite(boost::system::error_code error, size_t bytes) noexcept;

    tcp::socket & socket() noexcept;

    void close() noexcept;

 private:
    tcp::socket mSocket;
    boost::asio::io_service::strand mStrand;
    boost::asio::streambuf mReadBuffer;
    EventPointer mEvents;
};

}  // namespace quotes
