#include "quotes/server.h"

#include <time.h>
#include <map>
#include <random>

#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>

#include "quotes.pb.h"
#include "quotes/asset.h"
#include "quotes/log.h"
#include "quotes/session.h"
#include "quotes/types.h"

namespace quotes {

Server::Server(const Setup & setup)
    : mIoService{setup.service}, mAcceptor{*setup.service, setup.endpoint},
      mAssetsManager{setup.dataPath} {}

ServerPointer Server::create(const Setup & setup) {
    auto serverPointer = ServerPointer{new Server{setup}};
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
    from->send(answer);
}

proto::Packet Server::processAdd(const proto::Packet & packet) noexcept {
    auto answer = packet;
    answer.clear_assets();

    for (auto & packetAsset : packet.assets()) {
        std::vector<std::string> assetsNames{packetAsset.name()};
        if (packetAsset.name() == ALL_QUOTES_REQUEST)
            assetsNames = mAssetsManager.getAllNames();

        for (auto & assetName : assetsNames) {
            auto & asset = mAssetsManager.getOrCreate(assetName);
            auto protoAsset = answer.add_assets();
            protoAsset->set_name(assetName);

            for (auto & point : packetAsset.history()) {
                auto protoHistoryPoint = protoAsset->add_history();
                (*protoHistoryPoint) = point;
                asset.add(point);
            }
        }
    }

    return answer;
}

proto::Packet Server::processDelete(const proto::Packet & packet) noexcept {
    auto answer = packet;
    answer.clear_assets();

    for (auto & packetAsset : packet.assets()) {
        std::vector<std::string> assetsNames{packetAsset.name()};
        if (packetAsset.name() == ALL_QUOTES_REQUEST)
            assetsNames = mAssetsManager.getAllNames();

        for (auto & assetName : assetsNames) {
            if (mAssetsManager.remove(assetName)) {
                auto protoAsset = answer.add_assets();
                protoAsset->set_name(assetName);
                log("Delete", assetName);
            }
        }
    }

    return answer;
}

proto::Packet Server::processGet(const proto::Packet & packet) noexcept {
    std::map<std::string, Asset> requestedAssets;
    auto answer = packet;
    answer.clear_assets();

    for (auto & protoAsset : packet.assets()) {
        std::vector<std::string> assetsNames{protoAsset.name()};
        bool isRequestsAll = protoAsset.name() == ALL_QUOTES_REQUEST;

        if (isRequestsAll)
            assetsNames = mAssetsManager.getAllNames();

        for (auto & assetName : assetsNames) {
            if (!mAssetsManager.has(assetName))
                continue;

            if (isRequestsAll &&
                requestedAssets.find(assetName) == requestedAssets.end()) {
                auto asset = mAssetsManager.getOrCreate(assetName);

                if (protoAsset.history_size() == 1) {
                    auto size = protoAsset.history(0).value();
                    asset.truncate(size);
                    log("Request last", size, "points of", assetName);
                } else if (protoAsset.history_size() == 2) {
                    auto from = protoAsset.history(0).time();
                    auto to = protoAsset.history(1).time();
                    log("Request", assetName, "by time from", from, "to", to);
                    asset.truncate(from, to);
                } else {
                    asset.truncate(0);
                }

                requestedAssets.insert({assetName, asset});
            }
        }
    }

    for (auto & pair : requestedAssets) {
        auto newProtoAsset = answer.add_assets();
        (*newProtoAsset) = pair.second.toProto();
    }

    return answer;
}

void Server::onSessionDisconnected(SessionPointer session) noexcept {
    mClientsManager.removeClient(session);
}

}  // namespace quotes
