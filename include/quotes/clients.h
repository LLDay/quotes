#pragma once

#include <map>
#include <mutex>

#include "quotes/types.h"

namespace quotes {

class ClientsManager {
 public:
    ClientsManager() = default;

    void addClient(ClientId id, SessionPointer session);

    bool removeClient(ClientId id);

 private:
    std::map<ClientId, SessionPointer> mClients;
    std::mutex mMutex;
};

}  // namespace quotes
