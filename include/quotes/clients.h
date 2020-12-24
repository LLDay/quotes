#pragma once

#include <map>
#include <mutex>

#include "quotes/session.h"
#include "quotes/types.h"

namespace quotes {

class ClientsManager {
 public:
    ClientsManager() = default;

    void addClient(ClientId id, Session::pointer session);

    bool removeClient(ClientId id);

 private:
    std::map<ClientId, Session::pointer> mClients;
    std::mutex mMutex;
};

}  // namespace quotes
