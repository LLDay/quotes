#include "quotes/server.h"

#include <time.h>
#include <iostream>
#include <random>

#include <boost/asio/placeholders.hpp>
#include <boost/bind/bind.hpp>

#include "quotes/server.h"
#include "quotes/session.h"
#include "quotes/types.h"

namespace quotes {

Server::Server(Service service) noexcept
    : mIoService{service}, mAcceptor{*service, tcp::endpoint(tcp::v4(), 1100)} {
    startAccept();
}

ServerPointer Server::create(Service service) noexcept {
    return ServerPointer{new Server{std::move(service)}};
}

void Server::startAccept() noexcept {
    auto session = Session::create(mIoService, weak_from_this());
    auto bind = boost::bind(
        &Server::handleAccept, this, session, boost::asio::placeholders::error);
    mAcceptor.async_accept(session->socket(), bind);
}

void Server::handleAccept(
    SessionPointer session,
    const error_code & error) noexcept {
    if (!error) {
        unsigned int seed = time(nullptr);
        auto id = rand_r(&seed);
        mClientsManager.addClient(id, session);
        session->startReading();
    }
    startAccept();
}

void Server::onPacketRead(
    SessionPointer from,
    const proto::Packet & packet) noexcept {}

void Server::onSessionDisconnected(SessionPointer session) noexcept {
    std::cout << "Disconnected" << std::endl;
}

}  // namespace quotes
