#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace quotes {

using boost::asio::ip::tcp;

class Session : public boost::enable_shared_from_this<Session> {
 public:
    using pointer = boost::shared_ptr<Session>;
    using Service = boost::shared_ptr<boost::asio::io_service>;

 private:
    explicit Session(Service ioService) noexcept;

 public:
    static pointer create(Service ioService) noexcept;

    void startReading() noexcept;

    void startWriting() noexcept;

    void handleRead() noexcept;

    void handleWrite() noexcept;

    tcp::socket & socket() noexcept;

 private:
    tcp::socket mSocket;
};

}  // namespace quotes
