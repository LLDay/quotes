#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/system/error_code.hpp>

#include "quotes/types.h"

namespace quotes {

namespace proto {
class Packet;
}

using boost::asio::ip::tcp;

class Session : public boost::enable_shared_from_this<Session> {
 public:
    using pointer = boost::shared_ptr<Session>;

 private:
    explicit Session(Service ioService) noexcept;

 public:
    static pointer create(Service ioService) noexcept;

    void startReading() noexcept;

    void write(const proto::Packet & packet) noexcept;

    void handleRead(boost::system::error_code code, size_t bytes) noexcept;

    void handleWrite(boost::system::error_code code, size_t bytes) noexcept;

    tcp::socket & socket() noexcept;

 private:
    tcp::socket mSocket;
    boost::asio::streambuf mReadBuffer;
};

}  // namespace quotes
