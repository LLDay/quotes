#include "quotes/server.h"

#include <time.h>
#include <random>

#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>

#include "quotes/asset.h"
#include "quotes/server.h"
#include "quotes/session.h"
#include "quotes/types.h"

namespace quotes {

Server::Server(Service service, const tcp::endpoint & endpoint)
    : mIoService{service}, mAcceptor{*service, endpoint} {}

ServerPointer Server::create(Service service, const tcp::endpoint & endpoint) {
    auto serverPointer =
        ServerPointer{new Server{std::move(service), endpoint}};
    serverPointer->startAccept();
    return serverPointer;
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
    const proto::Packet & packet) noexcept {
    proto::Packet answer;
    switch (packet.type()) {
        case proto::Type::ADD: answer = processAdd(packet); break;
        case proto::Type::GET: answer = processGet(packet); break;
        case proto::Type::DELETE: answer = processDelete(packet); break;
    }

    answer.set_source(proto::Source::SERVER);
    from->send(packet);
}

proto::Packet Server::processAdd(const proto::Packet & packet) noexcept {
    for (auto i = 0; i < packet.assets_size(); ++i) {
        auto packetAsset = packet.assets(i);
        auto asset = mAssetsManager.getOrCreate(packetAsset.name());

        for (auto & point : packetAsset.history())
            asset.add(point);
    }

    return packet;
}
proto::Packet Server::processDelete(const proto::Packet & packet) noexcept {
    auto answer = packet;
    answer.clear_assets();

    for (auto i = 0; i < packet.assets_size(); ++i) {
        auto assetName = packet.assets(i).name();
        if (mAssetsManager.remove(assetName)) {
            auto protoAsset = answer.add_assets();
            protoAsset->set_name(assetName);
        }
    }

    return answer;
}

proto::Packet Server::processGet(const proto::Packet & packet) noexcept {
    auto answer = packet;
    answer.clear_assets();

    for (size_t i = 0; i < packet.assets_size(); ++i) {
        auto protoAsset = packet.assets(i);
        auto assetName = protoAsset.name();
        if (!mAssetsManager.has(assetName))
            continue;

        auto asset = mAssetsManager.getOrCreate(assetName);
        if (protoAsset.history_size() == 1) {
            auto size = protoAsset.history(0).value();
            asset = asset.truncate(size);
        } else if (protoAsset.history_size() == 2) {
            auto from = protoAsset.history(0).time();
            auto to = protoAsset.history(1).time();
            asset = asset.truncate(from, to);
        }

        auto newProtoAsset = answer.add_assets();
        (*newProtoAsset) = asset.toProto();
    }

    return answer;
}

void Server::onSessionDisconnected(SessionPointer session) noexcept {
    mClientsManager.removeClient(session);
}

}  // namespace quotes
