#pragma once

#include <map>
#include <mutex>

#include "quotes/session.h"

namespace quotes {

class ClientManager {
 public:
    using ClientId = int;

 public:
    ClientManager() = default;

    void addClient(ClientId id, Session::pointer session);

    bool removeClient(ClientId id);

 private:
    std::map<ClientId, Session::pointer> mClients;
    std::mutex mMutex;
};

}  // namespace quotes
