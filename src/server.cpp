#include "quotes/server.h"

#include <time.h>
#include <random>

#include <boost/asio/placeholders.hpp>
#include <boost/bind/bind.hpp>

namespace quotes {

Server::Server(ServicePointer service) noexcept
    : mIoService{service}, mAcceptor{*service, tcp::endpoint(tcp::v4(), 1100)} {
    startAccept();
}

void Server::startAccept() noexcept {
    auto session = Session::create(mIoService);
    auto bind = boost::bind(
        &Server::handleAccept, this, session, boost::asio::placeholders::error);
    mAcceptor.async_accept(session->socket(), bind);
}

void Server::handleAccept(
    Session::pointer session,
    const error_code & error) noexcept {
    if (!error) {
        unsigned int seed = time(nullptr);
        auto id = rand_r(&seed);
        mClientsManager.addClient(id, session);
        session->startReading();
    }
    startAccept();
}

ClientsManager & Server::manager() noexcept {
    return mClientsManager;
}

}  // namespace quotes
