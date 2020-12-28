#include "quotes/clients_manager.h"

#include <algorithm>
#include <iostream>

#include <boost/thread/lock_guard.hpp>

#include "quotes/types.h"

namespace quotes {

void ClientsManager::addClient(ClientId id, SessionPointer session) noexcept {
    boost::lock_guard lock{mMutex};
    mClients.insert({id, std::move(session)});
}

void clientRemoved(ClientId id) {
    std::cout << "Client " << id << " is disconnected" << std::endl;
}

bool ClientsManager::removeClient(ClientId id) noexcept {
    boost::lock_guard lock{mMutex};
    auto element = mClients.find(id);

    auto contains = element != mClients.end();
    if (!contains)
        return false;

    mClients.erase(element);
    clientRemoved(id);
    return true;
}

bool ClientsManager::removeClient(SessionPointer session) noexcept {
    boost::lock_guard lock{mMutex};
    for (auto & pair : mClients)
        if (pair.second == session) {
            mClients.erase(pair.first);
            clientRemoved(pair.first);
            return true;
        }

    return false;
}

}  // namespace quotes
