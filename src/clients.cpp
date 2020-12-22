#include "quotes/clients.h"

namespace quotes {

void ClientManager::addClient(ClientId id, Session::pointer session) {
    std::lock_guard lock{mMutex};
    mClients.insert({id, std::move(session)});
}

bool ClientManager::removeClient(ClientId id) {
    std::lock_guard lock{mMutex};
    auto element = mClients.find(id);
    auto contains = element != mClients.end();
    if (!contains)
        return false;

    mClients.erase(element);
    return true;
}

}  // namespace quotes
