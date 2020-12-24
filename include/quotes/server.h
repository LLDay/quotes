#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/detail/error_code.hpp>

#include "quotes/assets.h"
#include "quotes/clients.h"
#include "quotes/session.h"

namespace quotes {

using boost::asio::io_service;
using boost::asio::ip::tcp;
using boost::system::error_code;
using ServicePointer = boost::shared_ptr<io_service>;

class Server {
 public:
    explicit Server(ServicePointer ioService) noexcept;

    void startAccept() noexcept;

    void handleAccept(
        Session::pointer session,
        const error_code & error) noexcept;

    ClientsManager & manager() noexcept;

 private:
    ServicePointer mIoService;
    tcp::acceptor mAcceptor;
    ClientsManager mClientsManager;
    AssetsManager mAssetsManager;
};

}  // namespace quotes
