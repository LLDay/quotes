#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/system/detail/error_code.hpp>

#include "quotes.pb.h"
#include "quotes/assets_manager.h"
#include "quotes/clients_manager.h"
#include "quotes/events.h"
#include "quotes/types.h"

namespace quotes {

struct Setup {
    boost::asio::ip::tcp::endpoint endpoint;
    std::string dataPath;
    Service service;
    size_t threads;
};

using boost::asio::io_service;
using boost::asio::ip::tcp;
using boost::system::error_code;

class Server : public IEvents {
 private:
    constexpr static char const * ALL_QUOTES_REQUEST = "*";

    explicit Server(const Setup & setup);

 public:
    static ServerPointer create(const Setup & setup);

 private:
    void startAccept() noexcept;

    void handleAccept(
        SessionPointer session,
        const error_code & error) noexcept;

    void onPacketRead(
        SessionPointer from,
        const proto::Packet & packet) noexcept override;

    void onSessionDisconnected(SessionPointer session) noexcept override;

    proto::Packet processAdd(const proto::Packet & packet) noexcept;

    proto::Packet processDelete(const proto::Packet & packet) noexcept;

    proto::Packet processGet(const proto::Packet & packet) noexcept;

 private:
    Service mIoService;
    tcp::acceptor mAcceptor;
    ClientsManager mClientsManager;
    AssetsManager mAssetsManager;
};

}  // namespace quotes
